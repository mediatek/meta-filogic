DESCRIPTION = "testmode daemon for nl80211"
SECTION = "applications"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

DEPENDS += "libnl-tiny util-linux "
RDEPENDS:${PN} += "bash"
inherit pkgconfig cmake

SRC_URI = " \
    file://COPYING;subdir=${BP}/src \
    file://src;subdir=${BP} \
    file://ated.sh;subdir=${BP} \
    file://iwpriv.sh;subdir=${BP} \
    file://001-RDKB-ash-to-bash.patch;apply=no \
    "

S = "${UNPACKDIR}/${PN}-${PV}/src"

CFLAGS:append = " -I=${includedir}/libnl-tiny "

do_mtk_patches() {
	cd ${S}/../
    
	if [ ! -e mtk_wifi_patch_applied ]; then
        patch -p1 < ${UNPACKDIR}/001-RDKB-ash-to-bash.patch
	fi
	touch mtk_wifi_patch_applied
}
addtask mtk_patches after do_patch before do_configure

do_install:append() {
    install -d ${D}${sbindir}
    install -m 0755 ${UNPACKDIR}/${PN}-${PV}/ated.sh ${D}${sbindir}/ated
    install -m 0755 ${UNPACKDIR}/${PN}-${PV}/iwpriv.sh ${D}${sbindir}/iwpriv
    install -m 0755 ${UNPACKDIR}/${PN}-${PV}/iwpriv.sh ${D}${sbindir}/mwctl
}

