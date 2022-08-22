#!/bin/sh

wan_ifname=`syscfg get wan_physical_ifname`
sec_key=AD7A2BD03EAC835A6F620FDCB506B345

ip link add link $wan_ifname name macsec0 type macsec sci 12153524C0895E81 cipher gcm-aes-128 icvlen 8 encrypt off send_sci on end_station off scb off protect off replay off window 5 validate strict encodingsa 2
ip macsec add macsec0 rx sci 12153524C0895E81 on
ip macsec add macsec0 rx sci 12153524C0895E81 sa 2 pn 0xB2C28464 on key 01 $sec_key
ip macsec add macsec0 tx sa 2 pn 0xB2C28464 on key 02 $sec_key
ip link set macsec0 up
