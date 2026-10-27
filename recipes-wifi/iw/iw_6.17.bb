SUMMARY = "nl80211 based CLI configuration utility for wireless devices"
DESCRIPTION = "iw is a new nl80211 based CLI configuration utility for \
wireless devices. It supports almost all new drivers that have been added \
to the kernel recently. "
HOMEPAGE = "https://wireless.wiki.kernel.org/en/users/documentation/iw"
SECTION = "base"
LICENSE = "BSD-2-Clause"
LIC_FILES_CHKSUM = "file://COPYING;md5=878618a5c4af25e9b93ef0be1a93f774"

DEPENDS = "libnl"

SRC_URI = "http://www.kernel.org/pub/software/network/iw/${BP}.tar.xz \
           file://0001-iw-version.sh-don-t-use-git-describe-for-versioning.patch \
           file://separate-objdir.patch \
"

SRC_URI[sha256sum] = "7d182e498289ab39b257da6780d562e415377107f50358ee5b55b8cfe40b1e33"

PATCH_SRC = "${@bb.utils.contains('DISTRO_FEATURES', 'wifi_eht', 'patches-mlo', 'patches', d)}"

SRC_URI:remove:kernelv6 = "http://www.kernel.org/pub/software/network/iw/${BP}.tar.xz"
SRC_URI:append:kernelv6 = " \
    git://github.com/mediatek/softmac_iw.git;protocol=https;branch=mp4.3-6.12;destsuffix=${BPN}-${PV} \
    "
SRCREV:kernelv6 = "${AUTOREV}"
FILESEXTRAPATHS:prepend := "${THISDIR}/${PATCH_SRC}:"
require ${@bb.utils.contains('DISTRO_FEATURES','kernelv6','','${PATCH_SRC}/patches.inc',d)}
ERROR_QA:remove = "patch-fuzz patch-status"

inherit pkgconfig


EXTRA_OEMAKE = "\
    -f '${S}/Makefile' \
    \
    'PREFIX=${prefix}' \
    'SBINDIR=${sbindir}' \
    'MANDIR=${mandir}' \
"

do_install() {
    oe_runmake 'DESTDIR=${D}' install
}
