#!/bin/sh
set +e

export PATH=/sbin:/bin:/usr/sbin:/usr/bin
MOUNT="/bin/mount"
UMOUNT="/bin/umount"
INIT="/lib/systemd/systemd"

$MOUNT -o remount,rw /

mkdir -p /proc
mkdir -p /sys
mkdir -p /rdklogs

$MOUNT -t proc proc -o rw,nosuid,nodev,noexec,noatime /proc
$MOUNT -t sysfs sysfs -o rw,nosuid,nodev,noexec,noatime /sys
$MOUNT -n -t tmpfs tmpfs -o rw,nosuid,nodev,noexec,noatime /rdklogs


[ -z "$CONSOLE" ] && CONSOLE="/dev/console"
mkdir -p /mnt
dual_boot=$([ -f /sys/firmware/devicetree/base/mediatek,dual-boot ] && echo Y)

if [ x"${dual_boot}" = xY ]; then
	data_dev=$(blkid -t "PARTLABEL=rootfs_data" -o device)

	mkfs.f2fs -q -l rootfs_data $data_dev

	$MOUNT -n -t f2fs $data_dev -o rw,noatime /overlay
	
	if [ -f "/overlay/upper/reset-default" ]; then
		echo "Proceed with reset to default"
		$UMOUNT /overlay
		mkfs.f2fs -q -f -l rootfs_data $data_dev
		$MOUNT -n -t f2fs $data_dev -o rw,noatime /overlay
	fi
else
	mkfs.f2fs -q -l rootfs_data /dev/fitrw

	$MOUNT -n -t f2fs /dev/fitrw -o rw,noatime /overlay

	if [ -f "/overlay/upper/reset-default" ]; then
		echo "Proceed with reset to default"
		$UMOUNT /overlay
		mkfs.f2fs -q -f -l rootfs_data /dev/fitrw
		$MOUNT -n -t f2fs /dev/fitrw -o rw,noatime /overlay
	fi
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
