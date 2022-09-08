#!/bin/sh
set +e

export PATH=/sbin:/bin:/usr/sbin:/usr/bin
MOUNT="/bin/mount"
UMOUNT="/bin/umount"
INIT="/lib/systemd/systemd"


nand_find_volume() {
	local ubidevdir ubivoldir
	ubidevdir="/sys/devices/virtual/ubi/$1"
	[ ! -d "$ubidevdir" ] && return 1
	for ubivoldir in $ubidevdir/${1}_*; do
		[ ! -d "$ubivoldir" ] && continue
		if [ "$( cat $ubivoldir/name )" = "$2" ]; then
			basename $ubivoldir          
			return 0
		fi
	done
}


$MOUNT -o remount,rw /

mkdir -p /proc
mkdir -p /sys
mkdir -p /rdklogs

$MOUNT -t proc proc -o rw,nosuid,nodev,noexec,noatime /proc
$MOUNT -t sysfs sysfs -o rw,nosuid,nodev,noexec,noatime /sys
$MOUNT -n -t tmpfs tmpfs -o rw,nosuid,nodev,noexec,noatime /rdklogs

data_ubivol="$( nand_find_volume ubi0 rootfs_data )"

[ -z "$CONSOLE" ] && CONSOLE="/dev/console"
mkdir -p /mnt

$MOUNT -n -t ubifs /dev/$data_ubivol -o rw,noatime /overlay

if [ -f "/overlay/upper/reset-default" ]; then
	v "Proceed with reset to default"
	$UMOUNT /overlay
	rootfs_data_length=$(cat /sys/class/ubi/$data_ubivol/data_bytes)
	ubirmvol /dev/ubi0 -N rootfs_data
	ubimkvol /dev/ubi0 -N rootfs_data -s $rootfs_data_length
	$MOUNT -n -t ubifs /dev/$data_ubivol -o rw,noatime /overlay
fi

[ ! -d  "/overlay/upper" ] && mkdir /overlay/upper
[ ! -d  "/overlay/work" ] && mkdir /overlay/work

$MOUNT -n  -t overlay overlayfs:/overlay -o rw,noatime,lowerdir=/,upperdir=/overlay/upper,workdir=/overlay/work /mnt
$MOUNT -n /proc -o noatime --move /mnt/proc  
pivot_root /mnt /mnt/rom


$MOUNT -n /rom/sys -o noatime --move /sys
$MOUNT -n /rom/dev -o noatime --move /dev
$MOUNT -n /rom/rdklogs -o noatime --move /rdklogs
$MOUNT -n /rom/overlay -o noatime --move /overlay




exec $INIT
