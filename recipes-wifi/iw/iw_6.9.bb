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

SRC_URI[sha256sum] = "3f2db22ad41c675242b98ae3942dbf3112548c60a42ff739210f2de4e98e4894"

PATCH_SRC = "${@bb.utils.contains('DISTRO_FEATURES', 'wifi_eht', 'patches-mlo', 'patches', d)}"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PATCH_SRC}:"
require ${PATCH_SRC}/patches.inc

SRC_URI_remove = "${@bb.utils.contains('DISTRO_FEATURES', 'wifi_eht', 'file://001-nl80211_h_sync.patch', '', d)}"
SRC_URI_remove = "${@bb.utils.contains('DISTRO_FEATURES', 'wifi_eht', 'file://120-antenna_gain.patch', '', d)}"

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
