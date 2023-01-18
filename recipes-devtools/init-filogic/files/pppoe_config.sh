#!/bin/bash
source /etc/utopia/service.d/service_wan/ppp_helpers.sh

PPP_CONFIG_FILE=/etc/ppp/pppoe.conf

init_pppoe (){
    prepare_pppd_ip_pre_up_script
    prepare_pppd_ip_up_script
    prepare_pppd_ipv6_up_script
    prepare_pppd_ip_down_script
    prepare_pppd_ipv6_down_script
    prepare_pppd_options
    prepare_pppd_secrets
    echo -n > $PPP_CONFIG_FILE

    PHY_NAME=`syscfg get wan_physical_ifname`
    echo "ETH='${PHY_NAME}'" >> $PPP_CONFIG_FILE
    CLIENT=`syscfg get wan_proto_username`
    IPV6CP=`syscfg get IPV6CPEn`
    IPCP=`syscfg get IPCPEn`
    PPP_IDLE_TIME=`syscfg get ppp_idle_time`
    MAXMRUSIZE=`syscfg get MaxMRUSize`
    DEFLCPINTERVAL=`syscfg get lcp_interval`
    echo "MRU=$MAXMRUSIZE" >> $PPP_CONFIG_FILE
    echo "USER=$CLIENT" >>$PPP_CONFIG_FILE
    echo "IFNAME='ppp0'" >>$PPP_CONFIG_FILE
    echo "DEMAND=no" >> $PPP_CONFIG_FILE
    echo "DNSTYPE=SERVER" >> $PPP_CONFIG_FILE
    echo "PEERDNS=yes" >> $PPP_CONFIG_FILE
    echo "DNS1=" >> $PPP_CONFIG_FILE
    echo "DNS2=" >> $PPP_CONFIG_FILE
    echo "DEFAULTROUTE=yes" >> $PPP_CONFIG_FILE
    echo "CONNECT_TIMEOUT=0" >> $PPP_CONFIG_FILE
    echo "CONNECT_POLL=2" >> $PPP_CONFIG_FILE
    CONCENTRATOR=`syscfg get wan_proto_acname`
    echo "ACNAME=$CONCENTRATOR" >> $PPP_CONFIG_FILE
    SERVICE=`syscfg get wan_proto_servicename`
    echo "SERVICENAME=$SERVICE" >> $PPP_CONFIG_FILE
    echo 'PING="."' >> $PPP_CONFIG_FILE
    echo "CF_BASE=`basename $CONFIG`" >> $PPP_CONFIG_FILE
    echo 'PIDFILE="/var/run/$CF_BASE-pppoe.pid"' >> $PPP_CONFIG_FILE
    echo "SYNCHRONOUS=no" >> $PPP_CONFIG_FILE
    echo "CLAMPMSS=1412" >> $PPP_CONFIG_FILE
    echo "LCP_INTERVAL=$DEFLCPINTERVAL" >> $PPP_CONFIG_FILE
    echo "LCP_FAILURE=3" >> $PPP_CONFIG_FILE
    echo "PPPOE_TIMEOUT=80" >> $PPP_CONFIG_FILE
    echo "FIREWALL=NONE" >> $PPP_CONFIG_FILE
    echo 'LINUX_PLUGIN="/usr/lib/pppd/2.4.8/rp-pppoe.so"' >> $PPP_CONFIG_FILE
    echo 'PPPOE_EXTRA=""' >> $PPP_CONFIG_FILE
    echo 'PPPD_EXTRA=""' >> $PPP_CONFIG_FILE
}

if [ "$1" == "start" ]; then
    if [ -n "$2" ]; then
        echo "wan_proto_username"
        syscfg set wan_proto_username $2
    fi
    if [ -n "$3" ]; then
        echo "wan_proto_username"
        syscfg set wan_proto_password $3
    fi
    pppoe-stop
    sleep 2    
    init_pppoe
    pppoe-start
    iptables -t nat -D POSTROUTING -o ppp+ -j MASQUERADE
    iptables -t nat -I POSTROUTING -o ppp+ -j MASQUERADE
elif [ "$1" == "stop" ]; then
    pppoe-stop
    iptables -t nat -D POSTROUTING -o ppp+ -j MASQUERADE
else 
    echo "Useage: $0 <start/stop> <user> <password>"        
fi
