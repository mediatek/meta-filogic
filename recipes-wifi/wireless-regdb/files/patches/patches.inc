#patch patches (come from openwrt/lede/target/linux/mediatek)
SRC_URI_append = " \
    file://500-world-regd-5GHz.patch \
    file://600-world-regd-6GHz.patch \
    file://700-world-regd-5G_UNII-4.patch \
    file://999-add-country-vv.patch \
    "
