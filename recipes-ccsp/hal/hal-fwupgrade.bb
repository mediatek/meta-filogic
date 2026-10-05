SUMMARY = "HAL for RDK CCSP components"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://../../LICENSE;md5=175792518e4ac015ab6696d16c4f607e"
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
PROVIDES = "hal-fwupgrade"
RPROVIDES:${PN} = "hal-fwupgrade"

DEPENDS += "ccsp-common-library halinterface"
DEPENDS += " rdkb-halif-fwupgrade"
SRC_URI = "git://github.com/mediatek/rdkb_hal;branch=main;protocol=https;destsuffix=${BP};name=fwupgradehal \
           file://LICENSE;subdir=${UNPACKDIR}/${BP} \
          "

SRCREV_fwupgradehal = "${AUTOREV}"
SRCREV_FORMAT = "fwupgradehal"


S = "${UNPACKDIR}/${BP}/src/fwupgrade"

CFLAGS += "-DFEATURE_SUPPORT_RDKLOG"
CFLAGS:append = " -I=${includedir}/ccsp "

CFLAGS:append:wrynose = " -Wno-error=implicit-function-declaration -Wno-error=return-mismatch "

inherit autotools coverity

