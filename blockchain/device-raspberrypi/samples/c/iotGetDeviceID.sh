#!/bin/sh
FILE=/etc/iotsample-raspberrypi/device.cfg
if [ -e "$FILE" ]
then
	echo Running in registered mode
	cat $FILE
else
	LANG=C
	devId=`/sbin/ifconfig | grep 'eth0' | tr -s ' ' | cut -d ' ' -f5 |  tr -d ':'`
	echo The device ID is $devId
	echo For Real-time visualization of the data, visit http://quickstart.internetofthings.ibmcloud.com/?deviceId=$devId
        return 1
fi



