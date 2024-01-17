#!/bin/bash

if [ "$1" == "set" ]; then
    if [ ! -n "$2" ]; then
        echo "Error IP is necessary"
        exit 0
    fi
    LANIP=$2

    IP_PREFIX="${LANIP%.*}"

    MASK=255.255.255.0
    DHCP_START=$IP_PREFIX".2"
    DHCP_END=$IP_PREFIX".253"
    if [ -n "$3" ]; then
        MASK=$3
    fi
    if [ -n "$4" ]; then
        DHCP_START=$4
    fi
    if [ -n "$5" ]; then
        DHCP_END=$5
    fi  

    syscfg set lan_ipaddr $LANIP
    syscfg set lan_netmask $MASK
    syscfg set dhcp_start $DHCP_START
    syscfg set dhcp_end $DHCP_END
    syscfg commit
    sleep 1
    reboot
else
    echo "Useage: $0 <set> <ip> <mask> <dhcp_start> <dhcp_end>"
    echo "Example: $0 set 192.168.2.1"        
fi