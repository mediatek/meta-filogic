#!/bin/bash

if [ "$1" == "start" ]; then
    if [ ! -n "$2" ]; then
        echo "Error IP is necessary"
        exit 0
    fi
    if [ ! -n "$3" ]; then
        echo "Error netmask is necessary"
        exit 0
    fi
    if [ ! -n "$4" ]; then
        echo "Error gateway is necessary"
        exit 0
    fi
  
    ifconfig erouter0 $2 netmask $3 up
    route add default gw $4 dev erouter0
    iptables -t nat -D POSTROUTING -o erouter0 -j MASQUERADE
    iptables -t nat -I POSTROUTING -o erouter0 -j MASQUERADE
elif [ "$1" == "stop" ]; then
    ifconfig erouter0 0.0.0.0 up
    iptables -t nat -D POSTROUTING -o erouter0 -j MASQUERADE
else
    echo "Useage: $0 <start/stop> <ip> <mask> <gw>"        
fi