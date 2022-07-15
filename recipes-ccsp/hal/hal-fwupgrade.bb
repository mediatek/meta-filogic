SUMMARY = "HAL for RDK CCSP components"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://../../LICENSE;md5=175792518e4ac015ab6696d16c4f607e"
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
PROVIDES = "hal-fwupgrade"
RPROVIDES_${PN} = "hal-fwupgrade"

DEPENDS += "ccsp-common-library halinterface"
SRC_URI = "git://gerrit.mediatek.inc/gateway/rdk-b/rdkb_hal;protocol=https;name=fwupgradehal \
           file://LICENSE;subdir=git \
          "

SRCREV_fwupgradehal = "${AUTOREV}"
SRCREV_FORMAT = "fwupgradehal"


S = "${WORKDIR}/git/src/fwupgrade"

CFLAGS += "-DFEATURE_SUPPORT_RDKLOG"
CFLAGS_append = " -I=${includedir}/ccsp "

inherit autotools coverity

