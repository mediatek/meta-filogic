FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}-${PV}:"
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}-${PV}/generic/backport-6.6:"
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}-${PV}/generic/pending-6.6:"
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}-${PV}/generic/hack-6.6:"
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}-${PV}/mediatek/patches-6.6:"
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}-${PV}/mediatek/flow_patch:"

KBRANCH ?= "linux-6.6.y"

LINUX_VERSION ?= "6.6.92"
SRCREV_machine ?= "ffaf6178137b9cdcc9742d6677b70be164dfeb8c"
KMETA = "kernel-meta"
SRCREV_meta ?= "dff911ce87fe7b9944c6058907f079ddb0f3e840"

DEPENDS:append = " kern-tools-native xz-native bc-native"

SRC_URI = " \
    git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git;protocol=https;branch=${KBRANCH};name=machine \
    git://git.yoctoproject.org/yocto-kernel-cache;type=kmeta;name=meta;branch=yocto-6.6;destsuffix=${KMETA} \
    file://generic \
    file://mediatek \
    file://generic/defconfig \
    file://001-rdkb-eth-mtk-change-ifname-for.patch;apply=no \
    file://004-rdkb-hnat-bind-ifname.patch;apply=no \
    file://003-rdkb-refactor-bpi-r4-dts.patch;apply=no \
    "
SRC_URI:append_filogic += " \
    file://mediatek/filogic.cfg \
"

SRC_URI:append += " \
    file://rdkb_cfg/iptables.cfg \
    file://rdkb_cfg/turris_rdkb.cfg \
    file://rdkb_cfg/openvswitch.cfg \
    ${@bb.utils.contains('DISTRO_FEATURES', 'mt76', 'file://rdkb_cfg/mac80211.cfg', 'file://rdkb_cfg/nf_hnat.cfg', d)} \
    file://rdkb_cfg/prplmesh.cfg \
    file://rdkb_cfg/filogic_rdkb.cfg \
    file://rdkb_cfg/bridge_netfilter.cfg \
    ${@bb.utils.contains('DISTRO_FEATURES','emmc','file://rdkb_cfg/emmc.cfg','',d)} \
    ${@bb.utils.contains('DISTRO_FEATURES','ccn34','file://rdkb_cfg/tops.cfg','',d)} \
    ${@bb.utils.contains('DISTRO_FEATURES','ccn34','file://rdkb_cfg/eip-197.cfg','',d)} \
    ${@bb.utils.contains('DISTRO_FEATURES','usb3','file://rdkb_cfg/usb.cfg','',d)} \
"


require ${PN}-${PV}/generic/backport-6.6/backport-6.6.inc

require ${PN}-${PV}/generic/pending-6.6/pending-6.6.inc
SRC_URI:remove = " \
    file://530-jffs2_make_lzma_available.patch \
    "
require ${PN}-${PV}/generic/hack-6.6/hack-6.6.inc
SRC_URI:remove = " \
    file://531-debloat_lzma.patch \
    "
require ${PN}-${PV}/mediatek/patches-6.6/patches-6.6.inc


require linux-mediatek6-6.inc

export DTC_FLAGS = '-@'

do_patch:prepend () {
    cp -Rfp ${WORKDIR}/generic/files/* ${S}/
	cp -Rfp ${WORKDIR}/mediatek/files/* ${S}/
    cp -Rfp ${WORKDIR}/mediatek/files-6.6/* ${S}/
}

do_filogic_patches() {
    cd ${S}
    DISTRO_FlowBlock_ENABLED="${@bb.utils.contains('DISTRO_FEATURES','flow_offload','true','false',d)}"
    DISTRO_logan_ENABLED="${@bb.utils.contains('DISTRO_FEATURES','logan','true','false',d)}"
    DISTRO_ccn34_build_ENABLED="${@bb.utils.contains('DISTRO_FEATURES','ccn34','true','false',d)}"
    DISTRO_LAN_AS_WAN_ENABLED="${@bb.utils.contains('DISTRO_FEATURES','lan0_as_wan','true','false',d)}"
        if [ $DISTRO_ccn34_build_ENABLED = 'true' ]; then
            rm -rf ${WORKDIR}/mediatek/nf_hnat/999-41*.patch
            rm -rf ${WORKDIR}/mediatek/nf_hnat/999-45*.patch
        fi
        if [ ! -e patch_applied ]; then
            patch -p1 < ${WORKDIR}/001-rdkb-eth-mtk-change-ifname-for.patch
            patch -p1 < ${WORKDIR}/003-rdkb-refactor-bpi-r4-dts.patch

            patch -p1 < ${WORKDIR}/863-arm64-dts-mt7986-add-sound-wm8960.patch
            patch -p1 < ${WORKDIR}/999-2000-arm64-dts-mt7988-move-phys-to-sgmiipcs-and-usxgmiisy.patch
            patch -p1 < ${WORKDIR}/999-2757-net-dsa-add-netlink-support-for-an8855.patch
            patch -p1 < ${WORKDIR}/999-cpufreq-02-mediatek-enable-using-efuse-cali-data-for-mt7988-cpu-volt.patch

            if [ $DISTRO_FlowBlock_ENABLED = 'true' ]; then
                for i in ${WORKDIR}/mediatek/flow_patch/*.patch; do patch -p1 < $i; done
            fi
            if [ $DISTRO_logan_ENABLED = 'true' ]; then
                for i in ${WORKDIR}/mediatek/nf_hnat/*.patch; do patch -p1 < $i; done
                patch -p1 < ${WORKDIR}/004-rdkb-hnat-bind-ifname.patch
            fi
            touch patch_applied
        fi
}

kernel_do_install() {
	#
	# First install the modules
	#
	unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS MACHINE
	if (grep -q -i -e '^CONFIG_MODULES=y$' .config); then
		oe_runmake DEPMOD=echo MODLIB=${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION} INSTALL_FW_PATH=${D}${nonarch_base_libdir}/firmware modules_install
		rm "${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/build"
		#rm "${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/source"
		# If the kernel/ directory is empty remove it to prevent QA issues
		rmdir --ignore-fail-on-non-empty "${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/kernel"
	else
		bbnote "no modules to install"
	fi

	#
	# Install various kernel output (zImage, map file, config, module support files)
	#
	install -d ${D}/${KERNEL_IMAGEDEST}

	#
	# When including an initramfs bundle inside a FIT image, the fitImage is created after the install task
	# by do_assemble_fitimage_initramfs.
	# This happens after the generation of the initramfs bundle (done by do_bundle_initramfs).
	# So, at the level of the install task we should not try to install the fitImage. fitImage is still not
	# generated yet.
	# After the generation of the fitImage, the deploy task copies the fitImage from the build directory to
	# the deploy folder.
	#

	for imageType in ${KERNEL_IMAGETYPES} ; do
		if [ $imageType != "fitImage" ] || [ "${INITRAMFS_IMAGE_BUNDLE}" != "1" ] ; then
			install -m 0644 ${KERNEL_OUTPUT_DIR}/$imageType ${D}/${KERNEL_IMAGEDEST}/$imageType-${KERNEL_VERSION}
		fi
	done

	install -m 0644 System.map ${D}/${KERNEL_IMAGEDEST}/System.map-${KERNEL_VERSION}
	install -m 0644 .config ${D}/${KERNEL_IMAGEDEST}/config-${KERNEL_VERSION}
	install -m 0644 vmlinux ${D}/${KERNEL_IMAGEDEST}/vmlinux-${KERNEL_VERSION}
	[ -e Module.symvers ] && install -m 0644 Module.symvers ${D}/${KERNEL_IMAGEDEST}/Module.symvers-${KERNEL_VERSION}
	install -d ${D}${sysconfdir}/modules-load.d
	install -d ${D}${sysconfdir}/modprobe.d
}

addtask filogic_patches after do_patch before do_compile

KERNEL_MODULE_AUTOLOAD += "${@bb.utils.contains('DISTRO_FEATURES','logan','mtkhnat nf_flow_table_hw','',d)}"
KERNEL_MODULE_AUTOLOAD_mt7987 += "air_en8811h "