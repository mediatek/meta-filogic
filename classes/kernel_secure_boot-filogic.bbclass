inherit kernel-uboot kernel-artifact-names uboot-sign kernel-fitimage

python __anonymous () {
        d.appendVarFlag('do_gen_sb_dtb', 'depends', ' rdk-generic-broadband-image:do_hash_rootfs')
}

# Options for the device tree compiler passed to mkimage '-D' feature:
UBOOT_MKIMAGE_DTCOPTS ??= ""

# fitImage Hash Algo
FIT_HASH_ALG ?= "sha256"

# fitImage Signature Algo
FIT_SIGN_ALG ?= "rsa2048"

#
# Emit the fitImage ITS header
#
# $1 ... .its filename
fitimage_emit_fit_header() {
	cat << EOF >> ${1}
/dts-v1/;

/ {
        description = "U-Boot fitImage for ${DISTRO_NAME}/${PV}/${MACHINE}";
        #address-cells = <1>;
EOF
}

#
# Emit the fitImage section bits
#
# $1 ... .its filename
# $2 ... Section bit type: imagestart - image section start
#                          confstart  - configuration section start
#                          sectend    - section end
#                          fitend     - fitimage end
#
fitimage_emit_section_maint() {
	case $2 in
	imagestart)
		cat << EOF >> ${1}

        images {
EOF
	;;
	confstart)
		cat << EOF >> ${1}

        configurations {
EOF
	;;
	sectend)
		cat << EOF >> ${1}
	};
EOF
	;;
	fitend)
		cat << EOF >> ${1}
};
EOF
	;;
	esac
}

#
# Emit the fitImage ITS kernel section
#
# $1 ... .its filename
# $2 ... Image counter
# $3 ... Path to kernel image
# $4 ... Compression type
fitimage_emit_section_kernel() {

	kernel_csum="${FIT_HASH_ALG}"

	ENTRYPOINT="${UBOOT_ENTRYPOINT}"
	if [ -n "${UBOOT_ENTRYSYMBOL}" ]; then
		ENTRYPOINT=`${HOST_PREFIX}nm vmlinux | \
			awk '$3=="${UBOOT_ENTRYSYMBOL}" {print "0x"$1;exit}'`
	fi

	cat << EOF >> ${1}
                kernel-${2} {
                        description = "Linux kernel";
                        data = /incbin/("${3}");
                        type = "kernel";
                        arch = "${UBOOT_ARCH}";
                        os = "linux";
                        compression = "${4}";
                        load = <${UBOOT_LOADADDRESS}>;
                        entry = <${ENTRYPOINT}>;
                        hash-1 {
                                algo = "${kernel_csum}";
                        };
                };
EOF
}

#
# Emit the fitImage ITS DTB section
#
# $1 ... .its filename
# $2 ... Image counter
# $3 ... Path to DTB image
fitimage_emit_section_dtb() {

	dtb_csum="${FIT_HASH_ALG}"

	dtb_loadline=""
	dtb_ext=${DTB##*.}
	if [ "${dtb_ext}" = "dtbo" ]; then
		if [ -n "${UBOOT_DTBO_LOADADDRESS}" ]; then
			dtb_loadline="load = <${UBOOT_DTBO_LOADADDRESS}>;"
		fi
	elif [ -n "${UBOOT_DTB_LOADADDRESS}" ]; then
		dtb_loadline="load = <${UBOOT_DTB_LOADADDRESS}>;"
	fi
	cat << EOF >> ${1}
                fdt-${2} {
                        description = "Flattened Device Tree blob";
                        data = /incbin/("${3}");
                        type = "flat_dt";
                        arch = "${UBOOT_ARCH}";
                        compression = "none";
                        ${dtb_loadline}
                        hash-1 {
                                algo = "${dtb_csum}";
                        };
                };
EOF
}

#
# Emit the fitImage ITS setup section
#
# $1 ... .its filename
# $2 ... Image counter
# $3 ... Path to setup image
fitimage_emit_section_setup() {

	setup_csum="${FIT_HASH_ALG}"

	cat << EOF >> ${1}
                setup-${2} {
                        description = "Linux setup.bin";
                        data = /incbin/("${3}");
                        type = "x86_setup";
                        arch = "${UBOOT_ARCH}";
                        os = "linux";
                        compression = "none";
                        load = <0x00090000>;
                        entry = <0x00090000>;
                        hash-1 {
                                algo = "${setup_csum}";
                        };
                };
EOF
}

#
# Emit the fitImage ITS ramdisk section
#
# $1 ... .its filename
# $2 ... Image counter
# $3 ... Path to ramdisk image
fitimage_emit_section_ramdisk() {

	ramdisk_csum="${FIT_HASH_ALG}"
	ramdisk_loadline=""
	ramdisk_entryline=""

	if [ -n "${UBOOT_RD_LOADADDRESS}" ]; then
		ramdisk_loadline="load = <${UBOOT_RD_LOADADDRESS}>;"
	fi
	if [ -n "${UBOOT_RD_ENTRYPOINT}" ]; then
		ramdisk_entryline="entry = <${UBOOT_RD_ENTRYPOINT}>;"
	fi

	cat << EOF >> ${1}
                ramdisk-${2} {
                        description = "${INITRAMFS_IMAGE}";
                        data = /incbin/("${3}");
                        type = "ramdisk";
                        arch = "${UBOOT_ARCH}";
                        os = "linux";
                        compression = "none";
                        ${ramdisk_loadline}
                        ${ramdisk_entryline}
                        hash-1 {
                                algo = "${ramdisk_csum}";
                        };
                };
EOF
}

python do_gen_sb_dtb(){

    DEPLOY_DIR_IMAGE = d.getVar('DEPLOY_DIR_IMAGE', d, 1)
    SUMMARY_FILE="%s/hash-summary" %(DEPLOY_DIR_IMAGE)
    HASHED_BOOT_DEVICE="252:0"
    KERNEL_DEVICETREE = d.getVar('KERNEL_DEVICETREE', d, 1)
    dest = d.getVar('D', d, 1)
    build = d.getVar('B', d, 1)
    arch = d.getVar('ARCH', d, 1)
    KERNEL_IMAGEDEST = d.getVar('KERNEL_IMAGEDEST', d, 1)
    dtblist=KERNEL_DEVICETREE.split(" ")
    import os 
    for DTB in dtblist:
        if len(DTB) != 0:
            SecureDTB=DTB.replace(".dtb","-sb.dtb")
            BaseDTB=os.path.basename(DTB)
            BaseSecureDTB=BaseDTB.replace(".dtb","-sb.dtb")
            import time
            import subprocess
            subprocess.Popen("fdt-patch-dm-verify %s %s/arch/%s/boot/dts/%s %s/arch/%s/boot/dts/%s %s" %(SUMMARY_FILE, build, arch, DTB, build, arch, SecureDTB, HASHED_BOOT_DEVICE), shell=True)
            time.sleep( 1 )
            subprocess.Popen("install -m 0644 %s/arch/%s/boot/dts/%s %s/%s/%s" %(build, arch, SecureDTB, dest, KERNEL_IMAGEDEST, BaseSecureDTB), shell=True)
	
}

addtask gen_sb_dtb before do_deploy after do_install

#
# Emit the fitImage ITS configuration section
#
# $1 ... .its filename
# $2 ... Linux kernel ID
# $3 ... DTB image name
# $4 ... ramdisk ID
# $5 ... config ID
# $6 ... default flag
fitimage_emit_section_config_sb() {

	conf_csum="${FIT_HASH_ALG}"
	conf_sign_algo="${FIT_SIGN_ALG}"

	sb_sign_keyname="${SECURE_BOOT_KEYNAME}"


	# Test if we have any DTBs at all
	sep=""
	conf_desc=""
	kernel_line=""
	fdt_line=""
	ramdisk_line=""
	setup_line=""
	default_line=""

	if [ -n "${2}" ]; then
		conf_desc="Linux kernel"
		sep=", "
		kernel_line="kernel = \"kernel-${2}\";"
	fi

	if [ -n "${3}" ]; then
		conf_desc="${conf_desc}${sep}FDT blob"
		sep=", "
		fdt_line="fdt = \"fdt-${3}\";"
	fi

	if [ -n "${4}" ]; then
		conf_desc="${conf_desc}${sep}ramdisk"
		sep=", "
		ramdisk_line="ramdisk = \"ramdisk-${4}\";"
	fi

	if [ -n "${5}" ]; then
		conf_desc="${conf_desc}${sep}setup"
		setup_line="setup = \"setup-${5}\";"
	fi

	if [ "${6}" = "1" ]; then
		default_line="default = \"conf-${3}\";"
	fi

	cat << EOF >> ${1}
                ${default_line}
                conf-${3} {
			description = "${6} ${conf_desc}";
			${kernel_line}
			${fdt_line}
			${ramdisk_line}
			${setup_line}
                        hash-1 {
                                algo = "${conf_csum}";
                        };
EOF

	if [ ! -z "${sb_sign_keyname}" ] ; then

		sign_line="sign-images = "
		sep=""

		if [ -n "${2}" ]; then
			sign_line="${sign_line}${sep}\"kernel\""
			sep=", "
		fi

		if [ -n "${3}" ]; then
			sign_line="${sign_line}${sep}\"fdt\""
			sep=", "
		fi

		if [ -n "${4}" ]; then
			sign_line="${sign_line}${sep}\"ramdisk\""
			sep=", "
		fi

		if [ -n "${5}" ]; then
			sign_line="${sign_line}${sep}\"setup\""
		fi

		sign_line="${sign_line};"

		cat << EOF >> ${1}
                        signature-1 {
                                algo = "${conf_csum},${conf_sign_algo}";
                                key-name-hint = "${sb_sign_keyname}";
				${sign_line}
                        };
EOF
	fi

	cat << EOF >> ${1}
                };
EOF
}
#
# Assemble fitImage
#
# $1 ... .its filename
# $2 ... fitImage name
# $3 ... include ramdisk
fitimage_assemble_sb() {
	kernelcount=1
	dtbcount=""
	DTBS=""
	ramdiskcount=${3}
	setupcount=""
	rm -f ${1} arch/${ARCH}/boot/${2}

	fitimage_emit_fit_header ${1}

	#
	# Step 1: Prepare a kernel image section.
	#
	fitimage_emit_section_maint ${1} imagestart

	uboot_prep_kimage
	fitimage_emit_section_kernel ${1} "${kernelcount}" linux.bin "${linux_comp}"

	#
	# Step 2: Prepare a DTB image section
	#

	if [ -z "${EXTERNAL_KERNEL_DEVICETREE}" ] && [ -n "${KERNEL_DEVICETREE}" ]; then
		dtbcount=1
		for DTB in ${KERNEL_DEVICETREE}; do
			if echo ${DTB} | grep -q '/dts/'; then
				bbwarn "${DTB} contains the full path to the the dts file, but only the dtb name should be used."
				DTB=`basename ${DTB} | sed 's,\.dts$,.dtb,g'`
			fi
			DTB=`echo ${DTB} | sed 's,\.dtb$,-sb.dtb,g'`
			DTB_PATH="arch/${ARCH}/boot/dts/${DTB}"
			if [ ! -e "${DTB_PATH}" ]; then
				DTB_PATH="arch/${ARCH}/boot/${DTB}"
			fi

			DTB=$(echo "${DTB}" | tr '/' '_')
			DTBS="${DTBS} ${DTB}"
			fitimage_emit_section_dtb ${1} ${DTB} ${DTB_PATH}
		done
	fi

	if [ -n "${EXTERNAL_KERNEL_DEVICETREE}" ]; then
		dtbcount=1
		for DTB in $(find "${EXTERNAL_KERNEL_DEVICETREE}" \( -name '*.dtb' -o -name '*.dtbo' \) -printf '%P\n' | sort); do
			DTB=$(echo "${DTB}" | tr '/' '_')
			DTBS="${DTBS} ${DTB}"
			fitimage_emit_section_dtb ${1} ${DTB} "${EXTERNAL_KERNEL_DEVICETREE}/${DTB}"
		done
	fi

	#
	# Step 3: Prepare a setup section. (For x86)
	#
	if [ -e arch/${ARCH}/boot/setup.bin ]; then
		setupcount=1
		fitimage_emit_section_setup ${1} "${setupcount}" arch/${ARCH}/boot/setup.bin
	fi

	#
	# Step 4: Prepare a ramdisk section.
	#
	if [ "x${ramdiskcount}" = "x1" ] ; then
		# Find and use the first initramfs image archive type we find
		for img in cpio.lz4 cpio.lzo cpio.lzma cpio.xz cpio.gz ext2.gz cpio; do
			initramfs_path="${DEPLOY_DIR_IMAGE}/${INITRAMFS_IMAGE_NAME}.${img}"
			echo "Using $initramfs_path"
			if [ -e "${initramfs_path}" ]; then
				fitimage_emit_section_ramdisk ${1} "${ramdiskcount}" "${initramfs_path}"
				break
			fi
		done
	fi

	fitimage_emit_section_maint ${1} sectend

	# Force the first Kernel and DTB in the default config
	kernelcount=1
	if [ -n "${dtbcount}" ]; then
		dtbcount=1
	fi

	#
	# Step 5: Prepare a configurations section
	#
	fitimage_emit_section_maint ${1} confstart

	if [ -n "${DTBS}" ]; then
		i=1
		for DTB in ${DTBS}; do
			dtb_ext=${DTB##*.}
			if [ "${dtb_ext}" = "dtbo" ]; then
				fitimage_emit_section_config_sb ${1} "" "${DTB}" "" "" "`expr ${i} = ${dtbcount}`"
			else
				fitimage_emit_section_config_sb ${1} "${kernelcount}" "${DTB}" "${ramdiskcount}" "${setupcount}" "`expr ${i} = ${dtbcount}`"
			fi
			i=`expr ${i} + 1`
		done
	fi

	fitimage_emit_section_maint ${1} sectend

	fitimage_emit_section_maint ${1} fitend

	#
	# Step 6: Sign the image and add public key to U-Boot dtb
	#

	uboot-mkimage \
		${@'-D "${UBOOT_MKIMAGE_DTCOPTS}"' if len('${UBOOT_MKIMAGE_DTCOPTS}') else ''} \
		-f ${1} \
		-k "${SECURE_BOOT_KEYDIR}" \
		-r arch/${ARCH}/boot/${2}

}

python do_gen_sb_dtb(){

    DEPLOY_DIR_IMAGE = d.getVar('DEPLOY_DIR_IMAGE', d, 1)
    SUMMARY_FILE="%s/hash-summary" %(DEPLOY_DIR_IMAGE)
    HASHED_BOOT_DEVICE=d.getVar('HASHED_BOOT_DEVICE', d, 1)
    KERNEL_DEVICETREE = d.getVar('KERNEL_DEVICETREE', d, 1)
    DEST = d.getVar('D', d, 1)
    BUILD = d.getVar('B', d, 1)
    ARCH = d.getVar('ARCH', d, 1)
    KERNEL_IMAGEDEST = d.getVar('KERNEL_IMAGEDEST', d, 1)
    DTBLIST=KERNEL_DEVICETREE.split(" ")
    import os 
    for DTB in DTBLIST:
        if len(DTB) != 0:
            SecureDTB=DTB.replace(".dtb","-sb.dtb")
            BaseDTB=os.path.basename(DTB)
            BaseSecureDTB=BaseDTB.replace(".dtb","-sb.dtb")
            import time
            import subprocess
            subprocess.Popen("fdt-patch-dm-verify %s %s/arch/%s/boot/dts/%s %s/arch/%s/boot/dts/%s %s" %(SUMMARY_FILE, BUILD, ARCH, DTB, BUILD, ARCH, SecureDTB, HASHED_BOOT_DEVICE), shell=True)
            time.sleep( 1 )
            subprocess.Popen("install -m 0644 %s/arch/%s/boot/dts/%s %s/%s/%s" %(BUILD, ARCH, SecureDTB, DEST, KERNEL_IMAGEDEST, BaseSecureDTB), shell=True)
	
}

addtask gen_sb_dtb before do_deploy after do_install

do_assemble_secure_boot_fitimage() {
	if echo ${KERNEL_IMAGETYPES} | grep -wq "fitImage"; then
		cd ${B}
		fitimage_assemble_sb fit-image-sb.its fitImage-sb
	fi
}

addtask assemble_secure_boot_fitimage before do_deploy after do_gen_sb_dtb

python do_fit_image_sb_deploy(){
    DEPLOY_DIR_IMAGE = d.getVar('DEPLOY_DIR_IMAGE', d, 1)
    BUILD = d.getVar('B', d, 1)
    ARCH = d.getVar('ARCH', d, 1)
    import subprocess
    import time
    subprocess.Popen("rm %s/fitImage-sb" %(DEPLOY_DIR_IMAGE), shell=True)
    time.sleep( 1 )
    subprocess.Popen("install -m 0644 %s/arch/%s/boot/fitImage-sb %s/" %(BUILD, ARCH, DEPLOY_DIR_IMAGE), shell=True)
}
addtask fit_image_sb_deploy before do_deploy after do_assemble_secure_boot_fitimage

DEPENDS += "fdt-patch-dm-verify-native"