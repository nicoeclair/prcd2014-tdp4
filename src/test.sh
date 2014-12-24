#!/bin/bash

n=5
if [ "$1" != "" ]; then
	n="$1"
fi

i=3
while [ "$i" -le "$n" ]; do
	sed -i '2s/.*/1/' config
    echo -e "\n chinese"
	make exec np=${i} 1>/dev/null
	sed -i '2s/.*/0/' config
    echo -e "\n lin"
	make exec np=${i} 1>/dev/null
	i=$(($i+1))
done
