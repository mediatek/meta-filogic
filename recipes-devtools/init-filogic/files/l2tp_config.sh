#!/bin/bash
source /etc/utopia/service.d/service_wan/ppp_helpers.sh

DESIRED_WAN_STATE=`sysevent get desired_ipv4_wan_state`
CURRENT_WAN_STATE=`sysevent get current_ipv4_wan_state`
CURRENT_LINK_STATE=`sysevent get current_ipv4_link_state`
WAN_IFNAME=`sysevent get wan_ifname`
LAN_IFNAME=`syscfg get lan_ifname`

WAN_PROTOCOL=`syscfg get wan_proto`
CLIENT=`syscfg get wan_proto_username`
PASSWORD=`syscfg get wan_proto_password`
L2TP_CONF_DIR=/etc/xl2tpd
L2TP_CONF_FILE=$L2TP_CONF_DIR"/"xl2tpd.conf
L2TP_OPTIONS_DIR=/etc/ppp/peers
L2TP_OPTIONS_FILE=$L2TP_OPTIONS_DIR"/l2tp_tunnel"
PPP_OPTIONS_FILE=/etc/ppp/options
WAN_SERVER_IPADDR=`syscfg get wan_proto_server_address`

echo "[utopia][l2tp] Configuring l2tp" > /dev/console

# create the l2tp peers file

init_l2tp (){
    prepare_pppd_ip_pre_up_script
    prepare_pppd_ip_up_script
    prepare_pppd_ip_down_script
    prepare_pppd_ipv6_up_script
    prepare_pppd_ipv6_down_script
    
    prepare_pppd_secrets


    # create the l2tp conf file

    mkdir -p $L2TP_CONF_DIR
    echo -n > $PPP_OPTIONS_FILE
    echo -n > $L2TP_CONF_FILE

    # Global section (by default, we start in global mode)
    echo "[global]" >> $L2TP_CONF_FILE

    # Bind address
    echo "access control = no" >> $L2TP_CONF_FILE
    echo "port=1701" >> $L2TP_CONF_FILE

    echo "[lac l2tp]" >> $L2TP_CONF_FILE
    echo "name=l2tp" >> $L2TP_CONF_FILE
    L2TP_SERVER_IP=`syscfg get wan_proto_server_address`
    echo "lns=$L2TP_SERVER_IP" >> $L2TP_CONF_FILE
    echo "pppoptfile=/etc/ppp/peers/l2tp_tunnel" >> $L2TP_CONF_FILE
    echo "redial=yes" >> $L2TP_CONF_FILE
    echo "redial timeout=10" >> $L2TP_CONF_FILE
    echo "ppp debug=yes" >> $L2TP_CONF_FILE

    # create the l2tp option file
    echo "user $CLIENT" >> $L2TP_OPTIONS_FILE
    echo "password $PASSWORD" >> $L2TP_OPTIONS_FILE
    echo "persist" >> $L2TP_OPTIONS_FILE
    echo "nobsdcomp" >> $L2TP_OPTIONS_FILE
    #echo "noccp" >> $L2TP_OPTIONS_FILE
    echo "nopcomp" >> $L2TP_OPTIONS_FILE
    echo "noaccomp" >> $L2TP_OPTIONS_FILE
    echo "usepeerdns" >> $L2TP_OPTIONS_FILE
    echo "noipdefault" >> $L2TP_OPTIONS_FILE
    echo "require-mppe-128" >> $L2TP_OPTIONS_FILE
    echo "noauth" >> $L2TP_OPTIONS_FILE
    echo "debug" >> $L2TP_OPTIONS_FILE
    echo "dump" >> $L2TP_OPTIONS_FILE
    echo "logfd 2" >> $L2TP_OPTIONS_FILE
    echo "logfile /var/log/xl2tpd.log" >> $L2TP_OPTIONS_FILE
    echo "nodefaultroute" >> $L2TP_OPTIONS_FILE
    echo "ipparam 'l2tp'" >> $L2TP_OPTIONS_FILE
    echo "ifname 'l2tp-l2tp'" >> $L2TP_OPTIONS_FILE
    echo "lcp-max-terminate 0" >> $L2TP_OPTIONS_FILE
    echo "ktune" >> $L2TP_OPTIONS_FILE
    if [ ! -d "/var/run/xl2tpd" ]; then
        mkdir -p /var/run/xl2tpd
        touch /var/run/xl2tpd/l2tp-control
    fi    
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
    if [ -n "$4" ]; then
        echo "wan_proto_server_address"
        syscfg set wan_proto_server_address $4
    fi
  
    init_l2tp
    modprobe ppp_mppe > /dev/null 2>&1
    xl2tpd -D -p /var/run/xl2tpd.pid &
    sleep 1
    echo "c l2tp" > /var/run/xl2tpd/l2tp-control
elif [ "$1" == "stop" ]; then
    echo "d l2tp" > /var/run/xl2tpd/l2tp-control
    
else 
    echo "Useage: $0 <start/stop> <user> <password> <serv_ip>"        
fi