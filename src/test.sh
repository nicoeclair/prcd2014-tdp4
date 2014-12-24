#!/bin/bash

n=5
if [ "$1" != "" ]; then
	n="$1"
fi

i=3
while [ "$i" -le "$n" ]; do
	echo -e "$i"
    sum_sp=0
    sum_sp1=0
	sed -i '3s/.*/1/' config
    echo "Vol de travail"
	make exec np=${i}
	sed -i '3s/.*/0/' config
    echo "Sans vol"
	make exec np=${i}
	i=$(($i+1))
done
