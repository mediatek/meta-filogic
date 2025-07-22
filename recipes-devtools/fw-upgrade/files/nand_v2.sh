

#. /lib/functions.sh

# 'kernel' partition on NAND contains the kernel
CI_KERNPART="${CI_KERNPART:-kernel}"

# 'ubi' partition on NAND contains UBI
CI_UBIPART="${CI_UBIPART:-ubi}"

# 'rootfs' partition on NAND contains the rootfs
CI_ROOTPART="${CI_ROOTPART:-rootfs}"

find_mtd_index() {
	local PART="$(grep "\"$1\"" /proc/mtd | awk -F: '{print $1}')"
	local INDEX="${PART##mtd}"

	echo ${INDEX}
}

ubi_mknod() {
	local dir="$1"
	local dev="/dev/$(basename $dir)"

	[ -e "$dev" ] && return 0

	local devid="$(cat $dir/dev)"
	local major="${devid%%:*}"
	local minor="${devid##*:}"
	mknod "$dev" c $major $minor
}

nand_find_volume() {
	local ubidevdir ubivoldir
	ubidevdir="/sys/class/ubi/"
	[ ! -d "$ubidevdir" ] && return 1
	for ubivoldir in $ubidevdir/${1}_*; do
		[ ! -d "$ubivoldir" ] && continue
		if [ "$( cat $ubivoldir/name )" = "$2" ]; then
			basename $ubivoldir
			ubi_mknod "$ubivoldir"
			return 0
		fi
	done
}

nand_find_ubi() {
	local ubidevdir ubidev mtdnum cmtdnum
	mtdnum="$( find_mtd_index $1 )"
	[ ! "$mtdnum" ] && return 1
	for ubidevdir in /sys/class/ubi/ubi*; do
		[ ! -e "$ubidevdir/mtd_num" ] && continue
		cmtdnum="$( cat $ubidevdir/mtd_num )"
		if [ "$mtdnum" = "$cmtdnum" ]; then
			ubidev=$( basename $ubidevdir )
			ubi_mknod "$ubidevdir"
			echo $ubidev
			return 0
		fi
	done
}

nand_attach_ubi() {
	local ubipart="$1"
	local has_env="${2:-0}"

	local mtdnum="$( find_mtd_index "$ubipart" )"
	if [ ! "$mtdnum" ]; then
		>&2 echo "cannot find ubi mtd partition $ubipart"
		return 1
	fi

	local ubidev="$( nand_find_ubi "$ubipart" )"
	if [ ! "$ubidev" ]; then
		>&2 ubiattach -m "$mtdnum"
		ubidev="$( nand_find_ubi "$ubipart" )"

		if [ ! "$ubidev" ]; then
			>&2 ubiformat /dev/mtd$mtdnum -y
			>&2 ubiattach -m "$mtdnum"
			ubidev="$( nand_find_ubi "$ubipart" )"

			if [ ! "$ubidev" ]; then
				>&2 echo "cannot attach ubi mtd partition $ubipart"
				return 1
			fi

			if [ "$has_env" -gt 0 ]; then
				>&2 ubimkvol /dev/$ubidev -n 0 -N ubootenv -s 1MiB
				>&2 ubimkvol /dev/$ubidev -n 1 -N ubootenv2 -s 1MiB
			fi
		fi
	fi

	echo "$ubidev"
	return 0
}

nand_get_magic_long() {
	dd if="$1" skip=$2 bs=4 count=1 2>/dev/null | hexdump -v -n 4 -e '1/1 "%02x"'
}

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


identify() {
	identify_magic_long $(nand_get_magic_long "$1" "${2:-0}")
}

nand_restore_config() {
	sync
	local ubidev=$( nand_find_ubi $CI_UBIPART )
	local ubivol="$( nand_find_volume $ubidev rootfs_data )"
	[ ! "$ubivol" ] &&
		ubivol="$( nand_find_volume $ubidev $CI_ROOTPART )"
	mkdir /tmp/new_root
	if ! mount -t ubifs /dev/$ubivol /tmp/new_root; then
		echo "mounting ubifs $ubivol failed"
		rmdir /tmp/new_root
		return 1
	fi
	mv "$1" "/tmp/new_root/$BACKUP_FILE"
	umount /tmp/new_root
	sync
	rmdir /tmp/new_root
}

nand_remove_ubiblock() {
	local ubivol="$1"

	local ubiblk="ubiblock${ubivol:3}"
	if [ -e "/dev/$ubiblk" ]; then
		umount "/dev/$ubiblk" 2>/dev/null && echo "unmounted /dev/$ubiblk" || :
		if ! ubiblock -r "/dev/$ubivol"; then
			echo "cannot remove $ubiblk"
			return 1
		fi
	fi
}

nand_upgrade_prepare_ubi() {
	local rootfs_length="$1"
	local rootfs_type="$2"
	local rootfs_data_max="$(fw_printenv -n rootfs_data_max 2> /dev/null)"
	[ -n "$rootfs_data_max" ] && rootfs_data_max=$((rootfs_data_max))

	local kernel_length="$3"
	local has_env="${4:-0}"
	local kern_ubidev
	local root_ubidev

	[ -n "$rootfs_length" -o -n "$kernel_length" ] || return 1

	if [ -n "$CI_KERN_UBIPART" -a -n "$CI_ROOT_UBIPART" ]; then
		kern_ubidev="$( nand_attach_ubi "$CI_KERN_UBIPART" "$has_env" )"
		[ -n "$kern_ubidev" ] || return 1
		root_ubidev="$( nand_attach_ubi "$CI_ROOT_UBIPART" )"
		[ -n "$root_ubidev" ] || return 1
	else
		kern_ubidev="$( nand_attach_ubi "$CI_UBIPART" "$has_env" )"
		[ -n "$kern_ubidev" ] || return 1
		root_ubidev="$kern_ubidev"
	fi

	local kern_ubivol="$( nand_find_volume $kern_ubidev "$CI_KERNPART" )"
	local root_ubivol="$( nand_find_volume $root_ubidev "$CI_ROOTPART" )"
	local data_ubivol="$( nand_find_volume $root_ubidev rootfs_data )"
	[ "$root_ubivol" = "$kern_ubivol" ] && root_ubivol=

	# remove ubiblocks
	[ "$kern_ubivol" ] && { nand_remove_ubiblock $kern_ubivol || return 1; }
	[ "$root_ubivol" ] && { nand_remove_ubiblock $root_ubivol || return 1; }
	[ "$data_ubivol" ] && { nand_remove_ubiblock $data_ubivol || return 1; }

	# kill volumes
	[ "$kern_ubivol" ] && ubirmvol /dev/$kern_ubidev -N "$CI_KERNPART" || :
	[ "$root_ubivol" ] && ubirmvol /dev/$root_ubidev -N "$CI_ROOTPART" || :
	[ "$data_ubivol" ] && ubirmvol /dev/$root_ubidev -N rootfs_data || :

	# create kernel vol
	if [ -n "$kernel_length" ]; then
		if ! ubimkvol /dev/$kern_ubidev -N "$CI_KERNPART" -s $kernel_length; then
			echo "cannot create kernel volume"
			return 1;
		fi
	fi

	# create rootfs vol
	if [ -n "$rootfs_length" ]; then
		local rootfs_size_param
		if [ "$rootfs_type" = "ubifs" ]; then
			rootfs_size_param="-m"
		else
			rootfs_size_param="-s $rootfs_length"
		fi
		if ! ubimkvol /dev/$root_ubidev -N "$CI_ROOTPART" $rootfs_size_param; then
			echo "cannot create rootfs volume"
			return 1;
		fi
	fi

	# create rootfs_data vol for non-ubifs rootfs
	if [ "$rootfs_type" != "ubifs" ]; then
		local rootfs_data_size_param="-m"
		if [ -n "$rootfs_data_max" ]; then
			rootfs_data_size_param="-s $rootfs_data_max"
		fi
		if ! ubimkvol /dev/$root_ubidev -N rootfs_data $rootfs_data_size_param; then
			if ! ubimkvol /dev/$root_ubidev -N rootfs_data -m; then
				echo "cannot initialize rootfs_data volume"
				return 1
			fi
		fi
	fi

	return 0
}

nand_do_restore_config() {
	local conf_tar="/tmp/sysupgrade.tgz"
	[ ! -f "$conf_tar" ] || nand_restore_config "$conf_tar"
}

nand_do_upgrade_success() {
	if nand_do_restore_config && sync; then
		echo "sysupgrade successful"
		umount -a
		reboot -f
	fi
	nand_do_upgrade_failed
}

nand_do_upgrade_failed() {
	sync
	echo "sysupgrade failed"
	# Should we reboot or bring up some failsafe mode instead?
	umount -a
	reboot -f
}

ubi_dual_boot_do_restore_config() {
	local conf_tar="/tmp/sysupgrade.tgz"
	[ ! -f "$conf_tar" ] || ubi_dual_boot_restore_config "$conf_tar"
}

ubi_dual_boot_do_upgrade_success() {
	if ubi_dual_boot_do_restore_config && sync; then
		echo "sysupgrade successful"
		umount -a
		reboot -f
	fi
	nand_do_upgrade_failed
}


ubi_prepare_u_boot_env() {
	local ubidev="$1"

	local env_size=$(cat /sys/firmware/devicetree/base/mediatek,env-size 2>/dev/null)
	[ -z "${env_size}" ] && return

	local env_vol=$(cat /sys/firmware/devicetree/base/mediatek,env-ubi-volume 2>/dev/null)
	if [ -n "${env_vol}" ]; then
		local env_ubivol="$( nand_find_volume $ubidev $env_vol )"
		[ -z "$env_ubivol" ] && ubimkvol /dev/$ubidev -N ${env_vol} -s ${env_size} 2>/dev/null || :
	fi

	local env2_vol=$(cat /sys/firmware/devicetree/base/mediatek,env-ubi-volume-redund 2>/dev/null)
	if [ -n "${env2_vol}" ]; then
		local env2_ubivol="$( nand_find_volume $ubidev $env2_vol )"
		[ -z "$env2_ubivol" ] && ubimkvol /dev/$ubidev -N ${env2_vol} -s ${env_size} 2>/dev/null || :
	fi
}

# Write the FIT image to UBI kernel volume
nand_upgrade_itb() {
	local fit_file="$1"
	local gz="$2"

	local fit_length=$( (${gz}cat "$fit_file" | wc -c) 2> /dev/null)

	nand_upgrade_prepare_ubi "" "" "$fit_length" "" || return 1

	local fit_ubidev="$(nand_find_ubi "$CI_UBIPART")"
	local fit_ubivol="$(nand_find_volume $fit_ubidev "$CI_KERNPART")"
	${gz}cat "$fit_file" | ubiupdatevol /dev/$fit_ubivol -s "$fit_length" -

	# create u-boot environment volume
	ubi_prepare_u_boot_env $fit_ubidev
}

dual_boot_upgrade_prepare_ubi() {
	local boot_firmware_vol_name="$1"
	local firmware_vol_name="$2"
	local firmware_length="$3"
	local reserve_rootfs_data="$4"

	local ubidev="$( nand_attach_ubi "$CI_UBIPART" 0 )"

	local boot_fw_ubivol="$( nand_find_volume $ubidev $boot_firmware_vol_name )"
	local fw_ubivol="$( nand_find_volume $ubidev $firmware_vol_name )"
	local data_ubivol="$( nand_find_volume $ubidev rootfs_data )"

	# remove ubiblocks
	[ "$boot_fw_ubivol" ] && { nand_remove_ubiblock $boot_fw_ubivol || return 1; }
	[ "$fw_ubivol" ] && { nand_remove_ubiblock $fw_ubivol || return 1; }
	[ "$data_ubivol" ] && { nand_remove_ubiblock $data_ubivol || return 1; }

	# kill firmware volume
	[ "$fw_ubivol" ] && ubirmvol /dev/$ubidev -N "$firmware_vol_name" || :

	local rootfs_data_vol=$(cat /sys/firmware/devicetree/base/mediatek,upgrade-rootfs_data-part 2>/dev/null)
	if [ x"${reserve_rootfs_data}" != xY ]; then
		# kill rootfs_data volume
		[ "$data_ubivol" ] && ubirmvol /dev/$ubidev -N "$rootfs_data_vol" || :
	fi

	# create firmware vol
	if ! ubimkvol /dev/$ubidev -N "$firmware_vol_name" -s "$firmware_length"; then
		echo "cannot create firmware volume"
		return 1;
	fi

	# create u-boot environment volume
	ubi_prepare_u_boot_env $ubidev

	if [ x"${reserve_rootfs_data}" = xY ]; then
		# Do not touch rootfs_data
		sync
		return 0
	fi

	# create rootfs_data vol
	local rootfs_data_size=$(cat /sys/firmware/devicetree/base/mediatek,rootfs_data-size-limit 2>/dev/null)

	if [ -n "${rootfs_data_size}" ]; then
		rootfs_data_length="-s $rootfs_data_size"
	else
		rootfs_data_length="-m"
	fi

	if ! ubimkvol /dev/$ubidev -N "$rootfs_data_vol" "$rootfs_data_length"; then
		if [ -n "${rootfs_data_size}" ]; then
			if ! ubimkvol /dev/$root_ubidev -N "$rootfs_data_vol" -m; then
				echo "cannot initialize $rootfs_data_vol volume"
				return 1
			fi
		else
			echo "cannot initialize $rootfs_data_vol volume"
			return 1
		fi
	fi

	sync
	return 0
}

ubi_dual_boot_upgrade_itb() {
	local fit_file="$1"
	local gz="$2"

	local boot_firmware_vol_name=$(cat /sys/firmware/devicetree/base/mediatek,boot-firmware-part 2>/dev/null)
	[ -z "${boot_firmware_vol_name}" -o $? -ne 0 ] && return 1

	local firmware_vol_name=$(cat /sys/firmware/devicetree/base/mediatek,upgrade-firmware-part 2>/dev/null)
	[ -z "${firmware_vol_name}" -o $? -ne 0 ] && return 1

	local fit_length=$( (${gz}cat "$fit_file" | wc -c) 2> /dev/null)

	local reserve_rootfs_data=$([ -f /sys/firmware/devicetree/base/mediatek,reserve-rootfs_data ] && echo Y)
	dual_boot_upgrade_prepare_ubi "${boot_firmware_vol_name}" "${firmware_vol_name}" "${fit_length}" "${reserve_rootfs_data}" || return 1

	local fit_ubidev="$(nand_find_ubi "$CI_UBIPART")"
	local fit_ubivol="$(nand_find_volume $fit_ubidev "${firmware_vol_name}")"

	${gz}cat "$fit_file" | ubiupdatevol /dev/$fit_ubivol -s "$fit_length" - || return 1

	local upgrade_image_slot=$(cat /sys/firmware/devicetree/base/mediatek,upgrade-image-slot 2>/dev/null)
	if [ -n "${upgrade_image_slot}" ]; then
		v "Set new boot image slot to ${upgrade_image_slot}"
		# Force the creation of fw_printenv.lock
		mkdir -p /var/lock
		touch /var/lock/fw_printenv.lock
		fw_setenv "dual_boot.current_slot" "${upgrade_image_slot}"
		fw_setenv "dual_boot.slot_${upgrade_image_slot}_invalid" "0"
	fi

	if [ x"${reserve_rootfs_data}" != xY ]; then
		# do normal upgrade flow
		ubi_dual_boot_do_upgrade_success
	fi

	# Do not touch rootfs_data
	sync

	echo "sysupgrade successful"
	umount -a
	reboot -f
}

nand_verify_if_gzip_file() {
	local file="$1"
	local cmd="$2"

	if [ "$cmd" = zcat ]; then
		echo "verifying compressed sysupgrade file integrity"
		if ! gzip -t "$file"; then
			echo "corrupted compressed sysupgrade file"
			return 1
		fi
	fi
}

ubi_do_upgrade() {
	local file=$1
	local dual_boot=$([ -f /sys/firmware/devicetree/base/mediatek,dual-boot ] && echo Y)

	local file_type=$(identify $1)
	[ -e /dev/dm-0 ] && dmsetup remove_all
	[ -e /dev/fit0 ] && fitblk /dev/fit0
	[ -e /dev/fitrw ] && fitblk /dev/fitrw
	case "$file_type" in
		"fit")
			sync
			nand_verify_if_gzip_file "$file" "$gz" || return 1

			if [ x"${dual_boot}" != xY ]; then
				nand_upgrade_itb "$file" "$gz" && nand_do_upgrade_success
				nand_do_upgrade_failed
			else
				ubi_dual_boot_upgrade_itb "$file" "$gz"
			fi
			;;
		*)
			v "Unsupported firmware type: $file_type"
			;;
	esac
}
