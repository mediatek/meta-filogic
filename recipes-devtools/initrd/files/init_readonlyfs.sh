#!/bin/sh
set +e

export PATH=/sbin:/bin:/usr/sbin:/usr/bin
MOUNT="/bin/mount"
UMOUNT="/bin/umount"
INIT="/lib/systemd/systemd"

$MOUNT -n -t ubifs /dev/ubi0_2 /overlay
[ ! -d  "/overlay/upper" ] && mkdir /overlay/upper
[ ! -d  "/overlay/work" ] && mkdir /overlay/work
$MOUNT -n  -t overlay overlayfs:/overlay -o rw,noatime,lowerdir=/,upperdir=/overlay/upper,workdir=/overlay/work /mnt
pivot_root /mnt /mnt/rom
$MOUNT -n /rom/dev -o noatime --move /dev
$MOUNT -n /rom/overlay -o noatime --move /overlay


exec $INIT
