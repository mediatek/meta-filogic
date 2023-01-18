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

PPTP_OPTIONS_FILE=/etc/ppp/options.pptp
PPP_OPTIONS_FILE=/etc/ppp/options
WAN_SERVER_IPADDR=`syscfg get wan_proto_server_address`





init_pptp (){
    echo "[utopia][pptp] Configuring pptp" > /dev/console
    prepare_pppd_ip_pre_up_script
    prepare_pppd_ip_up_script
    prepare_pppd_ip_down_script
    prepare_pppd_ipv6_up_script
    prepare_pppd_ipv6_down_script
    
    prepare_pppd_secrets


    # create the l2tp conf file

    
    echo -n > $PPP_OPTIONS_FILE
    echo -n > $PPTP_OPTIONS_FILE


    # create the pptp option file
    echo "noauth" >> $PPTP_OPTIONS_FILE
    echo "refuse-eap" >> $PPTP_OPTIONS_FILE
    echo "user $CLIENT" >> $PPTP_OPTIONS_FILE
    echo "password $PASSWORD" >> $PPTP_OPTIONS_FILE
    echo "connect true" >> $PPTP_OPTIONS_FILE
    echo "pty 'pptp $WAN_SERVER_IPADDR --nolaunchpppd'" >> $PPTP_OPTIONS_FILE
    echo "lock" >> $PPTP_OPTIONS_FILE
    echo "maxfail 0" >> $PPTP_OPTIONS_FILE
    echo "usepeerdns" >> $PPTP_OPTIONS_FILE
	echo "persist" >> $PPTP_OPTIONS_FILE
	#echo "holdoff $PPTP_OPTIME" >> $PPTP_FILE
    echo "ipcp-accept-remote ipcp-accept-local noipdefault" >> $PPTP_OPTIONS_FILE
    echo "ktune" >> $PPTP_OPTIONS_FILE
    echo "default-asyncmap" >> $PPTP_OPTIONS_FILE
    echo "nopcomp" >> $PPTP_OPTIONS_FILE
    echo "noaccomp" >> $PPTP_OPTIONS_FILE
    echo "novj" >> $PPTP_OPTIONS_FILE
    echo "nobsdcomp" >> $PPTP_OPTIONS_FILE
    echo "nodeflate" >> $PPTP_OPTIONS_FILE
    echo "lcp-echo-interval 10" >> $PPTP_OPTIONS_FILE
    echo "lcp-echo-failure 6" >> $PPTP_OPTIONS_FILE

    echo "require-mppe-128" >> $PPTP_OPTIONS_FILE
    
    echo "debug" >> $PPTP_OPTIONS_FILE
    echo "dump" >> $PPTP_OPTIONS_FILE
    echo "logfd 2" >> $PPTP_OPTIONS_FILE
    echo "logfile /var/log/pptp.log" >> $PPTP_OPTIONS_FILE
    
    echo "ipparam 'pptp'" >> $PPTP_OPTIONS_FILE
    echo "ifname 'pptp-pptp'" >> $PPTP_OPTIONS_FILE
     
}

if [ "$1" == "start" ]; then
    if [ -n "$2" ]; then
        echo "wan_proto_username"
        syscfg set wan_proto_username $2
    fi
    if [ -n "$3" ]; then
        echo "wan_proto_password"
        syscfg set wan_proto_password $3
    fi
    if [ -n "$4" ]; then
        echo "wan_proto_server_address"
        syscfg set wan_proto_server_address $4
    fi
  
    init_pptp
    modprobe ppp_mppe > /dev/null 2>&1
    modprobe pptp > /dev/null 2>&1
    pppd file /etc/ppp/options.pptp &
elif [ "$1" == "stop" ]; then
    PID=`cat /var/run/pptp-pptp.pid`
    echo "PID $PID"
    kill $PID
else
    echo "Useage: $0 <start/stop> <user> <password> <serv_ip>"        
fi