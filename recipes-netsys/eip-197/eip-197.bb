SUMMARY = "Mediatek EIP-197 Driver"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit module

SRC_URI = "git://git01.mediatek.com/openwrt/feeds/mtk-openwrt-feeds;protocol=https;branch=master;destsuffix=git"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git/feed/kernel/crypto-eip/src"

DEPENDS += "pce"
DEPENDS += "${@bb.utils.contains('DISTRO_FEATURES', 'capwap-dtls', 'tops', '', d)}"

# There is dependency issue in this package.
# For now, we jsut disable parallel building as a workaround.
PARALLEL_MAKE = ""
BB_NUMBER_THREADS = "1"

do_configure[noexec] = "1"

# Include Paths
INCLUDE_FLAGS_ALL=" \
	-I${S}/ddk/inc \
	-I${S}/ddk/inc/crypto-eip/ddk \
	-I${S}/ddk/inc/crypto-eip/ddk/configs \
	-I${S}/ddk/inc/crypto-eip/ddk/shdevxs \
	-I${S}/ddk/inc/crypto-eip/ddk/umdevxs \
	-I${S}/ddk/inc/crypto-eip/ddk/device \
	-I${S}/ddk/inc/crypto-eip/ddk/device/lkm \
	-I${S}/ddk/inc/crypto-eip/ddk/device/lkm/of \
	-I${S}/ddk/inc/crypto-eip/ddk/dmares \
	-I${S}/ddk/inc/crypto-eip/ddk/firmware_api \
	-I${S}/ddk/inc/crypto-eip/ddk/kit/builder/sa \
	-I${S}/ddk/inc/crypto-eip/ddk/kit/builder/token \
	-I${S}/ddk/inc/crypto-eip/ddk/kit/eip197 \
	-I${S}/ddk/inc/crypto-eip/ddk/kit/iotoken \
	-I${S}/ddk/inc/crypto-eip/ddk/kit/list \
	-I${S}/ddk/inc/crypto-eip/ddk/kit/ring \
	-I${S}/ddk/inc/crypto-eip/ddk/libc \
	-I${S}/ddk/inc/crypto-eip/ddk/log \
	-I${S}/ddk/inc/crypto-eip/ddk/slad \
	-I${S}/ddk/inc/crypto-eip/ddk/slad/lkm \
	-I${STAGING_KERNEL_BUILDDIR}/source/drivers/net/ethernet/mediatek/ \
	-I${STAGING_KERNEL_BUILDDIR}/source/include \
	"

PKG_MAKE_FLAGS = " \
	CONFIG_MTK_CRYPTO_EIP_INLINE=m \
	CONFIG_RAMBUS_DDK=m \
	CONFIG_CRYPTO_XFRM_OFFLOAD_MTK_PCE=y \
	CONFIG_CRYPTO_OFFLOAD_INLINE=y \
	CONFIG_INET_ESP_OFFLOAD=y \
	CONFIG_CRYPTO_HW=y \
	CONFIG_CRYPTO_AUTHENC=y \
	CONFIG_CRYPTO_AES=y \
	CONFIG_CRYPTO_AEAD=y \
	CONFIG_CRYPTO_DES=y \
	CONFIG_CRYPTO_MD5=y \
	CONFIG_CRYPTO_SHA1=y \
	CONFIG_CRYPTO_SHA256=y \
	CONFIG_CRYPTO_SHA512=y \
	CONFIG_CRYPTO_SHA3=y \
	CONFIG_CRYPTO_HMAC=y \
	CONFIG_INET_ESP=y \
	"

EXTRA_CFLAGS+=" \
	${INCLUDE_FLAGS_ALL} \
	-DEIP197_BUS_VERSION_AXI3 \
	-DDRIVER_64BIT_HOST \
	-DDRIVER_64BIT_DEVICE \
	-DADAPTER_AUTO_TOKENBUILDER \
	-DCONFIG_XFRM_OFFLOAD \
	-DCONFIG_CRYPTO_XFRM_OFFLOAD_MTK_PCE \
	-DMODULE \
	"

include ${@bb.utils.contains('DISTRO_FEATURES', 'capwap-dtls', 'files/custom/eip-197-capwap-dtls.inc', '', d)}

EXTRA_OEMAKE = " \
	-C ${STAGING_KERNEL_BUILDDIR}/ \
	M=${S} \
	${PKG_MAKE_FLAGS} \
	EXTRA_CFLAGS='${EXTRA_CFLAGS}' \
	modules \
	"

do_install_append() {
	install -d ${D}/${base_libdir}/modules/${KERNEL_VERSION}/extra/
	install -m 0755 ${S}/crypto-eip-inline.ko ${D}/${base_libdir}/modules/${KERNEL_VERSION}/
	install -m 0755 ${S}/ddk/build/ksupport/crypto-eip-ddk-ksupport.ko ${D}/${base_libdir}/modules/${KERNEL_VERSION}/
	install -m 0755 ${S}/ddk/build/ctrl/crypto-eip-ddk-ctrl.ko ${D}/${base_libdir}/modules/${KERNEL_VERSION}/
	install -m 0755 ${S}/ddk/build/app/crypto-eip-ddk-ctrl-app.ko ${D}/${base_libdir}/modules/${KERNEL_VERSION}/
	install -m 0755 ${S}/ddk/build/engine/crypto-eip-ddk-engine.ko ${D}/${base_libdir}/modules/${KERNEL_VERSION}/
	# copy firmware and install
	install -d ${D}/${base_libdir}/firmware
	install -m 0755 ${S}/../firmware/bin/firmware_eip207_ifpp.bin ${D}/${base_libdir}/firmware/
	install -m 0755 ${S}/../firmware/bin/firmware_eip207_ipue.bin ${D}/${base_libdir}/firmware/
	install -m 0755 ${S}/../firmware/bin/firmware_eip207_ofpp.bin ${D}/${base_libdir}/firmware/
	install -m 0755 ${S}/../firmware/bin/firmware_eip207_opue.bin ${D}/${base_libdir}/firmware/
}

FILES_${PN} += "${base_libdir}/firmware/*"
