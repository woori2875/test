#!/usr/bin/bash

if [ ! -f "/system/fonts/opensans_regular.ttf" ]; then
    sleep 3
    mount -o remount,rw /system

    cp -rf /data/openpilot/installer/fonts/opensans* /system/fonts/
    cp -rf /data/openpilot/installer/fonts/fonts.xml /system/etc/fonts.xml
    chmod 644 /system/etc/fonts.xml
    chmod 644 /system/fonts/opensans*


    mount -o remount,r /system

    setprop persist.sys.locale ko-KR
    setprop persist.sys.local ko-KR
    setprop persist.sys.timezone Asia/Seoul

    echo =================================================================
    echo Ko-KR NanumGothic font install complete
    echo Ko-KR locale change complete
    echo =================================================================
    echo Reboot Now..!!
    echo =================================================================
    reboot
fi

export PASSIVE="0"
exec ./launch_chffrplus.sh
