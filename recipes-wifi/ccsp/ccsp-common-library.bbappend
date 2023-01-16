
do_filogic_wifi () {
    if [ -e "${D}${systemd_unitdir}/system/ccspwifiagent.service" ]; then
        sed -i "s/After=CcspPandMSsp.service/After=CcspPandMSsp.service hostapd.service/g" ${D}${systemd_unitdir}/system/ccspwifiagent.service
        sed -i "/EnvironmentFile/a ExecStartPre=/bin/sleep 7" ${D}${systemd_unitdir}/system/ccspwifiagent.service
    fi
}


addtask filogic_wifi after do_install before do_populate_sysroot