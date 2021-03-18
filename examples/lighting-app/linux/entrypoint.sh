#!/bin/bash

service dbus start
sleep 1
service avahi-daemon start
dbus-monitor --system --profile &
/usr/sbin/otbr-agent -d7 -v -I wpan0 spinel+hdlc+uart:///dev/ttyUSB0 &
sleep 1
ot-ctl panid 0x1234
ot-ctl ifconfig up
ot-ctl thread start

chip-lighting-app --thread
