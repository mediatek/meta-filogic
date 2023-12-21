#patch build (come from openwrt/lede/target/linux/mediatek)
SRC_URI_append = " \
    file://000-fix_kconfig.patch \
    file://001-fix_build.patch \
    file://002-change_allconfig.patch \
    file://003-remove_bogus_modparams.patch \
    file://004-fix-kconf-compiling.patch \
    file://012-kernel_build_check.patch \
    file://060-no_local_ssb_bcma.patch \
    file://070-remove-broken-wext-select.patch \
    file://080-resv_start_op.patch \
    file://090-bcma-otp.patch \
    file://100-backports-drop-QRTR-and-MHI.patch \
    file://110-backport_namepace_const.patch \
    file://120-headers_version_fix.patch \
    file://130-iommu_backport.patch \
    file://200-Revert-wifi-iwlwifi-Use-generic-thermal_zone_get_tri.patch \
    file://210-revert-split-op.patch \
    file://220-list-don-t-backport-list_count_nodes.patch \
    "
