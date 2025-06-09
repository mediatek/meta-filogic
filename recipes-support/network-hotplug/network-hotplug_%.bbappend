FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:remove = "file://network@.service"
SRC_URI:append = " file://filogic-network@.service"

do_install:append() {
    install -m 0644 ${WORKDIR}/filogic-network@.service ${D}${systemd_unitdir}/system/network@.service
}
