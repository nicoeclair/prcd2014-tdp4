#!/bin/bash

for i in 3 4 5 6
do
    sum_sp=0
    sum_sp1=0
	ret=`{ make exec np=${i} -B ;} 1>/dev/null`
	echo ${ret}
	ret=`{ make exec IMG_VERSION=img.mpi-thread.c np=${i} -B ;}  1>/dev/null`
	echo ${ret}
done
