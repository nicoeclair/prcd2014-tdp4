#!/bin/bash

for i in 3 4 5 6 7 8
do
    sum_sp=0
    sum_sp1=0
	sed -i '3s/.*/1/' config
    echo "Vol de travail"
	make exec np=${i}  1>/dev/null
	sed -i '3s/.*/0/' config
    echo "Simple répartition"
	make exec np=${i} 1>/dev/null
done
