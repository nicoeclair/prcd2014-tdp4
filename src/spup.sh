#!/bin/bash

for i in 3 4 5 6 7 8
do
    sum_sp=0
    sum_sp1=0
    echo "Vol de travail"
	ret=`{ make exec np=${i} -B  1>/dev/null;}`
	echo ${ret}
    echo "Simple répartition"
	ret=`{ make exec IMG_VERSION=img.mpi-thread.c np=${i} -B   1>/dev/null;}`
	echo ${ret}
done
