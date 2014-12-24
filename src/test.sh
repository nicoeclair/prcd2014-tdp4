#!/bin/bash

n=5
if [ "$1" != "" ]; then
	n="$1"
fi

i=3
while [ "$i" -le "$n" ]; do
    sum_sp=0
    sum_sp1=0
	sed -i '3s/.*/1/' config
    echo -e "\nVol de travail"
	make exec np=${i} 1>/dev/null
	sed -i '3s/.*/0/' config
    echo -e "\nSans vol"
	make exec np=${i} 1>/dev/null
	i=$(($i+1))
done
