SUMMARY = "fdt-patch-dm-verify"
SECTION = "applications"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"



SRC_URI = " \
    file://COPYING;subdir=git/src \
    file://src;subdir=git \
    "

S = "${WORKDIR}/git/src"

inherit native 

do_compile() {
    oe_runmake -C ${S} 
}

do_install() {
    install -d ${D}${sbindir}
    install -m 0755 ${S}/fdt-patch-dm-verify ${D}${sbindir}
}
BBCLASSEXTEND = "native nativesdk"