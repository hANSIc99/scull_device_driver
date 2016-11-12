#!/bin/bash

module="scull"
device="scull"
mode="664"

#herausfinden welche Gruppe die Distribution nutzt
#Ubuntu hat die Gruppe 'wheel'

if grep -q '^staff:' /etc/group; then
	group="staff"
else
	group="wheel"
fi

echo 'Gruppe: ' $group


# sudo ./scull_load.sh howmany=12 drv_name="Driver1" */
# '$*' entspricht den aufruf-argumenten
/sbin/insmod ~/Dokumente/kernel/staging/pwd/scull.ko $* || exit 1

echo 'Command line arguments:' $* || exit 1


# alte knoten entfernen
rm -f /dev/${device}[0-3]

#major-nummer herauslesen
major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)

echo 'Major: ' $major

#in /dev/ befinden sich jetzt 4 dateien
mknod /dev/${device}0 c $major 0
mknod /dev/${device}1 c $major 1
mknod /dev/${device}2 c $major 2
mknod /dev/${device}3 c $major 3



chgrp $group /dev/${device}[0-3]
chmod $mode /dev/${device}[0-3]
