# SPDX-License-Identifier: GPL-2.0-only
#
# Derived from meta/classes/kernel-fitimage.bbclass in
# https://code.rdkcentral.com/r/q/project:rdk/components/opensource/oe/openembedded-core
#
# Copyright (C) 2026 MediaTek Inc.
#

inherit kernel-uboot kernel-artifact-names uboot-sign kernel-fitimage

python __anonymous () {
    d.appendVarFlag('do_gen_sb_dtb', 'depends', ' rdk-generic-broadband-image:do_hash_rootfs')
}

UBOOT_SIGN_ENABLE = "1"
UBOOT_SIGN_KEYNAME = "${SECURE_BOOT_KEYNAME}"
UBOOT_SIGN_KEYDIR = "${SECURE_BOOT_KEYDIR}"

OFFLINE_SIGN_ENABLE ?= "0"

# Skip embedding public key to u-boot dtb
UBOOT_DTB_BINARY = ""

FIT_SIGN_ALG = "rsa2048"
FIT_HASH_ALG = "sha256"
FIT_PAD_ALG = "pss"

#
# Emit the fitImage ITS rootfs section
#
# $1 ... .its filename
# $2 ... Image counter
# $3 ... Path to rootfs image
fitimage_emit_section_rootfs() {
	rootfs_csum="${FIT_HASH_ALG}"

	cat << EOF >> ${1}
		rootfs-$2 {
			description = "ARM64 RDK-B rootfs";
			data = /incbin/("$3");
			type = "filesystem";
			arch = "${UBOOT_ARCH}";
			compression = "none";
			hash-1 {
				algo = "${rootfs_csum}";
			};
		};
EOF
}

#
# Emit the fitImage ITS configuration section
#
# $1 ... .its filename
# $2 ... Linux kernel ID
# $3 ... DTB image name
# $4 ... ramdisk ID
# $5 ... u-boot script ID
# $6 ... config ID
# $7 ... default flag
# $8 ... rootfs ID
fitimage_emit_section_config_itb() {

	conf_csum="${FIT_HASH_ALG}"
	conf_sign_algo="${FIT_SIGN_ALG}"
	conf_padding_algo="${FIT_PAD_ALG}"
	if [ "x${UBOOT_SIGN_ENABLE}" = "x1" ] ; then
		conf_sign_keyname="${UBOOT_SIGN_KEYNAME}"
	fi

	its_file="$1"
	kernel_id="$2"
	dtb_image="$3"
	ramdisk_id="$4"
	bootscr_id="$5"
	config_id="$6"
	default_flag="$7"
	rootfs_id="$8"
	dtb_filename="${dtb_image%.*}"

	# Test if we have any DTBs at all
	sep=""
	conf_desc=""
	conf_node=""
	kernel_line=""
	fdt_line=""
	ramdisk_line=""
	bootscr_line=""
	setup_line=""
	default_line=""
	loadables_line=""

	# conf node name is selected based on dtb ID if it is present,
	# otherwise its selected based on kernel ID
	if [ -n "$dtb_image" ]; then
		conf_node=${dtb_filename#mediatek_}
	else
		conf_node=$conf_node$kernel_id
	fi

	if [ -n "$kernel_id" ]; then
		conf_desc="Linux kernel"
		sep=", "
		kernel_line="kernel = \"kernel-$kernel_id\";"
	fi

	if [ -n "$dtb_image" ]; then
		conf_desc="$conf_desc${sep}FDT blob"
		sep=", "
		fdt_line="fdt = \"fdt-$dtb_image\";"
	fi

	if [ -n "$ramdisk_id" ]; then
		conf_desc="$conf_desc${sep}ramdisk"
		sep=", "
		ramdisk_line="ramdisk = \"ramdisk-$ramdisk_id\";"
	fi

	if [ -n "$bootscr_id" ]; then
		conf_desc="$conf_desc${sep}u-boot script"
		sep=", "
		bootscr_line="bootscr = \"bootscr-$bootscr_id\";"
	fi

	if [ -n "$config_id" ]; then
		conf_desc="$conf_desc${sep}setup"
		setup_line="setup = \"setup-$config_id\";"
	fi

	if [ -n "$rootfs_id" ]; then
		conf_desc="$conf_desc${sep}rootfs"
		sep=", "
		loadables_line="loadables = \"rootfs-$rootfs_id\";"
	fi

	if [ "x${default_flag}" = "x1" ]; then
		# default node is selected based on dtb ID if it is present,
		# otherwise its selected based on kernel ID
		if [ -n "$dtb_image" ]; then
			default_line="default = \"config-1\";"
			conf_node="config-1"
		else
			default_line="default = \"${FIT_CONF_PREFIX}$kernel_id\";"
		fi
	fi

	cat << EOF >> $its_file
                $default_line
                $conf_node {
                        description = "$default_flag $conf_desc";
                        $kernel_line
                        $fdt_line
                        $loadables_line
                        $ramdisk_line
                        $bootscr_line
                        $setup_line
                        hash-1 {
                                algo = "$conf_csum";
                        };
EOF

	if [ -n "$conf_sign_keyname" ] ; then

		sign_line="sign-images = "
		sep=""

		if [ -n "$kernel_id" ]; then
			sign_line="$sign_line${sep}\"kernel\""
			sep=", "
		fi

		if [ -n "$dtb_image" ]; then
			sign_line="$sign_line${sep}\"fdt\""
			sep=", "
		fi

		if [ -n "$ramdisk_id" ]; then
			sign_line="$sign_line${sep}\"ramdisk\""
			sep=", "
		fi

		if [ -n "$bootscr_id" ]; then
			sign_line="$sign_line${sep}\"bootscr\""
			sep=", "
		fi

		if [ -n "$config_id" ]; then
			sign_line="$sign_line${sep}\"setup\""
		fi

		if [ -n "$rootfs_id" ]; then
			sign_line="$sign_line${sep}\"loadables\""
			sep=", "
		fi

		sign_line="$sign_line;"

		cat << EOF >> $its_file
                        signature-1 {
                                algo = "$conf_csum,$conf_sign_algo";
                                key-name-hint = "$conf_sign_keyname";
                                padding = "$conf_padding_algo";
                                $sign_line
                        };
EOF
	fi

	cat << EOF >> $its_file
                };
EOF
}

#
# Assemble fitImage
#
# $1 ... .its filename
# $2 ... fitImage name
# $3 ... include ramdisk
# $4 ... include rootfs
fitimage_assemble_itb() {
	kernelcount=1
	dtbcount=""
	DTBS=""
	ramdiskcount=$3
	rootfscount=$4
	setupcount=""
	bootscr_id=""
	offline_flag=""
	rm -f $1 arch/${ARCH}/boot/$2

	if [ -n "${UBOOT_SIGN_IMG_KEYNAME}" -a "${UBOOT_SIGN_KEYNAME}" = "${UBOOT_SIGN_IMG_KEYNAME}" ]; then
		bbfatal "Keys used to sign images and configuration nodes must be different."
	fi

	fitimage_emit_fit_header $1

	#
	# Step 1: Prepare a kernel image section.
	#
	fitimage_emit_section_maint $1 imagestart

	uboot_prep_kimage
	fitimage_emit_section_kernel $1 $kernelcount linux.bin "$linux_comp"

	#
	# Step 2: Prepare a DTB image section
	#

	if [ -n "${KERNEL_DEVICETREE}" ]; then
		dtbcount=1
		for DTB in ${KERNEL_DEVICETREE}; do
			if echo $DTB | grep -q '/dts/'; then
				bbwarn "$DTB contains the full path to the the dts file, but only the dtb name should be used."
				DTB=`basename $DTB | sed 's,\.dts$,.dtb,g'`
			fi

			# Skip ${DTB} if it's also provided in ${EXTERNAL_KERNEL_DEVICETREE}
			if [ -n "${EXTERNAL_KERNEL_DEVICETREE}" ] && [ -s ${EXTERNAL_KERNEL_DEVICETREE}/${DTB} ]; then
				continue
			fi

			DTB=`echo ${DTB} | sed 's,\.dtb$,-sb.dtb,g'`
			DTB_PATH="arch/${ARCH}/boot/dts/$DTB"
			if [ ! -e "$DTB_PATH" ]; then
				DTB_PATH="arch/${ARCH}/boot/$DTB"
			fi

			DTB=$(echo "$DTB" | tr '/' '_')

			# Skip DTB if we've picked it up previously
			echo "$DTBS" | tr ' ' '\n' | grep -xq "$DTB" && continue

			DTBS="$DTBS $DTB"
			fitimage_emit_section_dtb $1 $DTB $DTB_PATH
		done
	fi

	if [ -n "${EXTERNAL_KERNEL_DEVICETREE}" ]; then
		dtbcount=1
		for DTB in $(find "${EXTERNAL_KERNEL_DEVICETREE}" \( -name '*.dtb' -o -name '*.dtbo' \) -printf '%P\n' | sort); do
			DTB=$(echo "$DTB" | tr '/' '_')

			# Skip DTB if we've picked it up previously
			echo "$DTBS" | tr ' ' '\n' | grep -xq "$DTB" && continue

			DTBS="$DTBS $DTB"
			fitimage_emit_section_dtb $1 $DTB "${EXTERNAL_KERNEL_DEVICETREE}/$DTB"
		done
	fi

	#
	# Step 3: Prepare a u-boot script section
	#

	if [ -n "${UBOOT_ENV}" ] && [ -d "${STAGING_DIR_HOST}/boot" ]; then
		if [ -e "${STAGING_DIR_HOST}/boot/${UBOOT_ENV_BINARY}" ]; then
			cp ${STAGING_DIR_HOST}/boot/${UBOOT_ENV_BINARY} ${B}
			bootscr_id="${UBOOT_ENV_BINARY}"
			fitimage_emit_section_boot_script $1 "$bootscr_id" ${UBOOT_ENV_BINARY}
		else
			bbwarn "${STAGING_DIR_HOST}/boot/${UBOOT_ENV_BINARY} not found."
		fi
	fi

	#
	# Step 4: Prepare a setup section. (For x86)
	#
	if [ -e arch/${ARCH}/boot/setup.bin ]; then
		setupcount=1
		fitimage_emit_section_setup $1 $setupcount arch/${ARCH}/boot/setup.bin
	fi

	#
	# Step 5: Prepare a ramdisk section.
	#
	if [ "x${ramdiskcount}" = "x1" ] && [ "${INITRAMFS_IMAGE_BUNDLE}" != "1" ]; then
		# Find and use the first initramfs image archive type we find
		found=
		for img in ${FIT_SUPPORTED_INITRAMFS_FSTYPES}; do
			initramfs_path="${DEPLOY_DIR_IMAGE}/${INITRAMFS_IMAGE_NAME}.$img"
			if [ -e "$initramfs_path" ]; then
				bbnote "Found initramfs image: $initramfs_path"
				found=true
				fitimage_emit_section_ramdisk $1 "$ramdiskcount" "$initramfs_path"
				break
			else
				bbnote "Did not find initramfs image: $initramfs_path"
			fi
		done

		if [ -z "$found" ]; then
			bbfatal "Could not find a valid initramfs type for ${INITRAMFS_IMAGE_NAME}, the supported types are: ${FIT_SUPPORTED_INITRAMFS_FSTYPES}"
		fi
	fi

	#
	# Step 6: Prepare a rootfs section.
	#
	if [ "x${rootfscount}" = "x1" ]; then
		filename="rdk-generic-broadband-image-${MACHINE}.squashfs-xz"
		rootfs_path="${DEPLOY_DIR_IMAGE}/${filename}"

		fitimage_emit_section_rootfs $1 $rootfscount "$rootfs_path"
	fi

	fitimage_emit_section_maint $1 sectend

	# Force the first Kernel and DTB in the default config
	kernelcount=1
	if [ -n "$dtbcount" ]; then
		dtbcount=1
	fi

	#
	# Step 7: Prepare a configurations section
	#
	fitimage_emit_section_maint $1 confstart

	# kernel-fitimage.bbclass currently only supports a single kernel (no less or
	# more) to be added to the FIT image along with 0 or more device trees and
	# 0 or 1 ramdisk.
	# It is also possible to include an initramfs bundle (kernel and rootfs in one binary)
	# When the initramfs bundle is used ramdisk is disabled.
	# If a device tree is to be part of the FIT image, then select
	# the default configuration to be used is based on the dtbcount. If there is
	# no dtb present than select the default configuation to be based on
	# the kernelcount.
	if [ -n "$DTBS" ]; then
		i=1
		for DTB in ${DTBS}; do
			dtb_ext=${DTB##*.}
			if [ "$dtb_ext" = "dtbo" ]; then
				fitimage_emit_section_config_itb $1 "" "$DTB" "" "$bootscr_id" "" "`expr $i = $dtbcount`" ""
			else
				fitimage_emit_section_config_itb $1 $kernelcount "$DTB" "$ramdiskcount" "$bootscr_id" "$setupcount" "`expr $i = $dtbcount`" $rootfscount
			fi
			i=`expr $i + 1`
		done
	else
		defaultconfigcount=1
		fitimage_emit_section_config_itb $1 $kernelcount "" "$ramdiskcount" "$bootscr_id"  "$setupcount" $defaultconfigcount ""
	fi

	fitimage_emit_section_maint $1 sectend

	fitimage_emit_section_maint $1 fitend

	#
	# Step 8: Assemble and sign the image and add public key to U-Boot dtb
	#
	if [ "x${UBOOT_SIGN_ENABLE}" = "x1" ] ; then
		add_key_to_u_boot=""
		if [ -n "${UBOOT_DTB_BINARY}" ]; then
			# The u-boot.dtb is a symlink to UBOOT_DTB_IMAGE, so we need copy
			# both of them, and don't dereference the symlink.
			cp -P ${STAGING_DATADIR}/u-boot*.dtb ${B}
			add_key_to_u_boot="-K ${B}/${UBOOT_DTB_BINARY}"
		fi

		if [ "x${OFFLINE_SIGN_ENABLE}" = "x1" ] ; then
			offline_flag=",offline"
		fi

		${UBOOT_MKIMAGE_SIGN} \
			${@'-D "${UBOOT_MKIMAGE_DTCOPTS}"' if len('${UBOOT_MKIMAGE_DTCOPTS}') else ''} \
			-E -B 0x1000 \
			-f $1 \
			-o "${FIT_HASH_ALG},${FIT_SIGN_ALG}${offline_flag}" \
			-k "${UBOOT_SIGN_KEYDIR}" \
			$add_key_to_u_boot \
			-r arch/${ARCH}/boot/$2 \
			${UBOOT_MKIMAGE_SIGN_ARGS}
	else
		#
		# Assemble the image only
		#
		${UBOOT_MKIMAGE} \
			${@'-D "${UBOOT_MKIMAGE_DTCOPTS}"' if len('${UBOOT_MKIMAGE_DTCOPTS}') else ''} \
			-E -B 0x1000 \
			-f $1 \
			arch/${ARCH}/boot/$2
	fi
}

def fdt_patch_dm_verity(d, dtb_path, out_dtb_path):
    import subprocess

    deploy_dir_image = d.getVar('DEPLOY_DIR_IMAGE')
    hashed_boot_device = d.getVar('HASHED_BOOT_DEVICE')
    summary_file = "%s/hash-summary" % deploy_dir_image

    cmd = "fdt-patch-dm-verify %s %s %s %s" % (
        summary_file, dtb_path, out_dtb_path, hashed_boot_device)
    subprocess.check_call(cmd, shell=True)

def fdt_patch_rootdev(d, dtb_path, bootargs_prop, rootdev, create):
    import os
    import re
    import subprocess

    staging_bindir_native = d.getVar('STAGING_BINDIR_NATIVE')
    fdtget = os.path.join(staging_bindir_native, "fdtget")
    fdtput = os.path.join(staging_bindir_native, "fdtput")

    def patch_node(dtb, node, prop, rdev, do_create):
        try:
            bootargs = subprocess.check_output(
                [fdtget, "-t", "s", dtb, node, prop],
                stderr=subprocess.DEVNULL).decode().strip()
        except subprocess.CalledProcessError:
            bootargs = ""

        if bootargs:
            new_bootargs = re.sub(r'root=[^ ]*', 'root=%s' % rdev, bootargs)
            if bootargs != new_bootargs:
                bb.note("set rootdev dtb=%s node=%s prop=%s val=\"%s\"" %
                        (dtb, node, prop, new_bootargs))
                subprocess.check_call(
                    [fdtput, "-t", "s", dtb, node, prop, new_bootargs])
        elif do_create:
            bb.note("set rootdev dtb=%s node=%s prop=%s val=\"root=%s\"" %
                    (dtb, node, prop, rdev))
            subprocess.check_call(
                [fdtput, "-t", "s", dtb, node, prop, "root=%s" % rdev])

    if not dtb_path.endswith('.dtbo'):
        patch_node(dtb_path, "/chosen", bootargs_prop, rootdev, create)
    else:
        try:
            nodes = subprocess.check_output(
                [fdtget, "-l", dtb_path, "/"],
                stderr=subprocess.DEVNULL).decode().strip().split('\n')
        except subprocess.CalledProcessError:
            return

        for node in nodes:
            if "fragment" not in node:
                continue
            try:
                target_path = subprocess.check_output(
                    [fdtget, "-t", "s", dtb_path, "/%s" % node, "target-path"],
                    stderr=subprocess.DEVNULL).decode().strip()
            except subprocess.CalledProcessError:
                continue
            if "/chosen" in target_path:
                patch_node(dtb_path, "/%s/__overlay__" % node,
                           bootargs_prop, rootdev, create)

python do_gen_sb_dtb () {
    import os
    import shutil

    arch = d.getVar('ARCH')
    build = d.getVar('B')
    dest = d.getVar('D')
    kernel_devicetree = d.getVar('KERNEL_DEVICETREE')
    kernel_imagedest = d.getVar('KERNEL_IMAGEDEST')
    rootdev = "/dev/dm-0"

    for dtb in kernel_devicetree.split():
        if dtb.endswith('.dtbo'):
            dtbo_path = "%s/arch/%s/boot/dts/%s" % (build, arch, dtb)
            if not os.path.exists(dtbo_path):
                continue

            fdt_patch_rootdev(d, dtbo_path, "bootargs-append", rootdev, False)
        else:
            orig_dtb_path = "%s/arch/%s/boot/dts/%s" % (build, arch, dtb)
            if not os.path.exists(orig_dtb_path):
                continue

            secure_dtb = dtb.replace(".dtb", "-sb.dtb")
            secure_dtb_path = "%s/arch/%s/boot/dts/%s" % (build, arch, secure_dtb)

            fdt_patch_dm_verity(d, orig_dtb_path, secure_dtb_path)

            base_secure_dtb = os.path.basename(secure_dtb)
            install_path = "%s/%s/%s" % (dest, kernel_imagedest, base_secure_dtb)
            shutil.copy2(secure_dtb_path, install_path)
}

addtask gen_sb_dtb before do_deploy after do_install

do_assemble_filogic_secure_boot_fitimage() {
	if echo ${KERNEL_IMAGETYPES} | grep -wq "fitImage"; then
		cd ${B}
		fitimage_assemble_itb fit-filogic-image-sb.its fitImage-filogic-sb "" "1"
	fi
}

addtask assemble_filogic_secure_boot_fitimage before do_deploy after do_gen_sb_dtb

python do_fit_image_filogic_secure_boot_deploy () {
    import os
    import shutil

    deploy_dir = d.getVar('DEPLOY_DIR_IMAGE')
    build_dir = d.getVar('B')
    arch = d.getVar('ARCH')

    fit_src = "%s/arch/%s/boot/fitImage-filogic-sb" % (build_dir, arch)
    fit_dst = "%s/fitImage-filogic-sb" % deploy_dir

    if os.path.exists(fit_dst):
        os.remove(fit_dst)

    shutil.copy2(fit_src, fit_dst)
}

addtask fit_image_filogic_secure_boot_deploy before do_deploy after do_assemble_filogic_secure_boot_fitimage

DEPENDS += "fdt-patch-dm-verify-native"
