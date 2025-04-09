FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI:append += "\
    file://remove_iwpriv.patch \
"