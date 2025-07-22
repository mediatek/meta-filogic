
# Keep these values be up-to-date with definition in libfstools/rootdisk.c of fstools package
board_name=`cat /proc/device-tree/compatible`


identify_magic_long() {
	local magic=$1
	case "$magic" in
		"55424923")
			echo "ubi"
			;;
		"31181006")
			echo "ubifs"
			;;
		"68737173")
			echo "squashfs"
			;;
		"d00dfeed")
			echo "fit"
			;;
		"4349"*)
			echo "combined"
			;;
		"1f8b"*)
			echo "gzip"
			;;
		*)
			echo "unknown $magic"
			;;
	esac
}

get_magic_long() {
	(get_image "$@" | dd bs=4 count=1 | hexdump -v -n 4 -e '1/1 "%02x"') 2>/dev/null
}

get_image() { # <source> [ <command> ]
	local from="$1"
	local cmd="$2"

	if [ -z "$cmd" ]; then
		local magic="$(dd if="$from" bs=2 count=1 2>/dev/null | hexdump -n 2 -e '1/1 "%02x"')"
		case "$magic" in
			1f8b) cmd="busybox zcat";;
			*) cmd="cat";;
		esac
	fi

	$cmd <"$from"
}

block_dev_path() {
	local dev_path

	case "$1" in
	/dev/mmcblk*)
		dev_path="$1"
		;;
	PARTLABEL=* | PARTUUID=*)
		dev_path=$(blkid -t "$1" -o device)
		[ -z "${dev_path}" -o $? -ne 0 ] && return 1
		;;
	*)
		return 1;
		;;
	esac

	echo "${dev_path}"
	return 0
}

emmc_upgrade_fit() {
	local fit_file="$1"
	[ "$CI_KERNPART" -a -z "$EMMC_KERN_DEV" ] && export EMMC_KERN_DEV="$(find_mmc_part $CI_KERNPART $CI_ROOTDEV)"

	if [ "$EMMC_KERN_DEV" ]; then
		export EMMC_KERNEL_BLOCKS=$(($(get_image "$fit_file" | fwtool -i /dev/null -T - | dd of="$EMMC_KERN_DEV" bs=512 2>&1 | grep "records out" | cut -d' ' -f1)))

		[ -z "$UPGRADE_BACKUP" ] && dd if=/dev/zero of="$EMMC_KERN_DEV" bs=512 seek=$EMMC_KERNEL_BLOCKS count=8
	fi
}

mmc_dual_boot_upgrade_itb() {
	local fit_file="$1"
	local reserve_rootfs_data=$([ -f /sys/firmware/devicetree/base/mediatek,reserve-rootfs_data ] && echo Y)

	local firmware_part=$(cat /sys/firmware/devicetree/base/mediatek,upgrade-firmware-part 2>/dev/null)
	[ -z "${firmware_part}" -o $? -ne 0 ] && return 1

	EMMC_KERN_DEV=$(block_dev_path "${firmware_part}")
	[ -z "${EMMC_KERN_DEV}" -o $? -ne 0 ] && return 1

	export EMMC_KERNEL_BLOCKS=$(($(get_image "$fit_file" | fwtool -i /dev/null -T - | dd of="$EMMC_KERN_DEV" bs=512 2>&1 | grep "records out" | cut -d' ' -f1)))

	local upgrade_image_slot=$(cat /sys/firmware/devicetree/base/mediatek,upgrade-image-slot 2>/dev/null)
	[ -n "${upgrade_image_slot}" ] && {
		v "Set new boot image slot to ${upgrade_image_slot}"
		# Force the creation of fw_printenv.lock
		mkdir -p /var/lock
		touch /var/lock/fw_printenv.lock
		fw_setenv "dual_boot.current_slot" "${upgrade_image_slot}"
		fw_setenv "dual_boot.slot_${upgrade_image_slot}_invalid" "0"
	}

	local rootfs_data_dev=$(cat /sys/firmware/devicetree/base/mediatek,rootfs_data-part 2>/dev/null)
	if [ -z "${rootfs_data_dev}" -o $? -ne 0 ]; then
		# Individual rootfs_data for each slot
		[ -z "$UPGRADE_BACKUP" ] && dd if=/dev/zero of="$EMMC_KERN_DEV" bs=512 seek=$EMMC_KERNEL_BLOCKS count=8
		return
	fi

	if [ x"${reserve_rootfs_data}" = xY ]; then
		# Do not touch rootfs_data
		return
	fi

	rootfs_data_dev=$(block_dev_path "${rootfs_data_dev}")
	[ -z "${rootfs_data_dev}" -o $? -ne 0 ] && return 1

	[ -z "$UPGRADE_BACKUP" ] && dd if=/dev/zero of="${rootfs_data_dev}" bs=512 count=8
}

mmc_do_upgrade() {
	local file=$1
	local dual_boot=$([ -f /sys/firmware/devicetree/base/mediatek,dual-boot ] && echo Y)

	local file_type=$(identify_magic_long "$(get_magic_long "$file")")
	case "$file_type" in
		"fit")
			if [ x"${dual_boot}" != xY ]; then
				emmc_upgrade_fit "$file"
				sync
				return
			fi

			mmc_dual_boot_upgrade_itb "$file"
			sync
			;;
		*)
			v "Unsupported firmware type: $file_type"
			;;
	esac
}
