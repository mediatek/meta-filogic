SUMMARY = "fitblk firmware release tool"
SECTION = "applications"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=c188eeeb69c0a05d0545816f1458a0c9"

SRC_URI = " \
    file://COPYING;subdir=${BP}/src \
    file://src;subdir=${BP} \
    "

S = "${UNPACKDIR}/${PN}-${PV}/src"

do_compile() {
    oe_runmake -C ${S} 
}

do_install() {
    install -d ${D}${sbindir}
    install -d ${D}${base_libdir}/upgrade
    install -m 0755 ${S}/fitblk ${D}${sbindir}
    install -m 0755 ${S}/fit.sh ${D}${base_libdir}/upgrade
}

FILES:${PN} += " \
            ${base_libdir}/upgrade/* \
            "
