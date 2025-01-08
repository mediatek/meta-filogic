SUMMARY = "Tool for adding rootfs hash node to FIT image"
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
    install -m 0755 ${S}/fit-rootfs-hash-tool ${D}${sbindir}
}
BBCLASSEXTEND = "native nativesdk"