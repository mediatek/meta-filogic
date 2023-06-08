#patch build (come from openwrt/lede/target/linux/mediatek)
SRC_URI_append = " \
    file://000-fix_kconfig.patch \
    file://001-fix_build.patch \
    file://002-change_allconfig.patch \
    file://003-remove_bogus_modparams.patch \
    file://012-kernel_build_check.patch \
    file://060-no_local_ssb_bcma.patch \
    file://070-remove-broken-wext-select.patch \
    file://080-resv_start_op.patch \
    file://090-bcma-otp.patch \
    file://100-backports-drop-QRTR-and-MHI.patch \
    file://110-backport_napi_build_skb.patch \
    "
