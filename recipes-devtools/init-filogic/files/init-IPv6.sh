#!/bin/sh

sysevent set multinet-up 1
/lib/rdk/dibbler-init.sh
sleep 2