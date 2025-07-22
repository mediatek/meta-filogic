
board_name=`cat /proc/device-tree/compatible`
. /lib/upgrade/nand.sh
. /lib/upgrade/mmc.sh
. /lib/upgrade/fit.sh

platform_do_upgrade() {
	local board=$board_name
	echo " board=$board_name ."
	case "$board" in
	mediatek,mt7981-rfb|\
	mediatek,mt7987*|\
	mediatek,mt7988a-rfb)
		export_fitblk_bootdev
		case "$CI_METHOD" in
		emmc)
			mmc_do_upgrade "$1"
			;;
		default)
			default_do_upgrade "$1"
			;;
		ubi)
			CI_KERNPART="firmware"
			ubi_do_upgrade "$1"
			;;
		*)
			if grep \"rootfs_data\" /proc/mtd; then
				default_do_upgrade "$1"
			fi
			;;
		esac
		;;
	*)
		nand_do_upgrade "$1"
		;;
	esac
}

 PART_NAME=firmware

platform_check_image() {
	local board=$board_name
	local magic="$(get_magic_long "$1")"

	[ "$#" -gt 1 ] && return 1

	case "$board" in
	mediatek,mt7981-rfb|\
	mediatek,mt7987*|\
	mediatek,mt7988a-rfb|\
	bananapi,bpi-r3|\
	bananapi,bpi-r3-mini|\
	bananapi,bpi-r4|\
	bananapi,bpi-r4-poe|\
	cmcc,rax3000m)
		[ "$magic" != "d00dfeed" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	*)
		nand_do_platform_check "$board" "$1"
		return $?
		;;
	esac

	return 0
}
