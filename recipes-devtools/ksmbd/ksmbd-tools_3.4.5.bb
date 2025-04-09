SUMMARY = " a collection of userspace utilities for the ksmbd kernel server"
SECTION = "Applications"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

inherit autotools pkgconfig systemd

DEPENDS += "ksmbd libnl glib-2.0"

SRC_URI = " \
    git://git@github.com/cifsd-team/ksmbd-tools.git;protocol=https;tag=${PV};branch=master \
    file://smb.conf \
    file://ksmbd.service \
    file://COPYING;subdir=git \
    "



S = "${WORKDIR}/git"

#EXTRA_OECONF += "GLIB_LIBS="${STAGING_LIBDIR}/libglib-2.0.a""
EXTRA_OECONF:append  = " --disable-shared --enable-static"
CFLAGS:prepend = " \
    -D_GNU_SOURCE \
    -I${STAGING_INCDIR}/libnl3 \
    -I${STAGING_INCDIR}/glib-2.0 \
    -I${STAGING_LIBDIR}/glib-2.0/include \
    -I${S} \
    -ffunction-sections -fdata-sections -flto \
    "
LDFLAGS += " -lglib-2.0"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE:${PN} = " ksmbd.service"
FILES:${PN} += "{systemd_unitdir}/system/ksmbd.service"

do_install:append () {
	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${WORKDIR}/ksmbd.service ${D}${systemd_unitdir}/system
    install -d ${D}/etc/ksmbd
    install -m 0644 ${WORKDIR}/smb.conf ${D}/etc/ksmbd
    install -m 0644 ${S}//Documentation/configuration.txt ${D}/etc/ksmbd
}