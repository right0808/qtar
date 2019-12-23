#!/bin/bash

[[ "$1" == "" || "$2" == "" ]] && {
    echo "usage: $0 /path/to/input-dir /path/to/output.tar.gz"
    exit
}

QTAR=`pwd`/qtar
INPUT_DIR=$1
OUTPUT_TAR_GZ=$2

cd $INPUT_DIR
SECONDS=0
sudo $QTAR . > $OUTPUT_TAR_GZ.files.list
T1=$SECONDS
echo "list files: $(($T1 / 60)) mins $(($T1 % 60)) secs"
SECONDS=0
sudo tar zcf $OUTPUT_TAR_GZ -T $OUTPUT_TAR_GZ.files.list --no-unquote
T2=$SECONDS
echo "tar zcf   : $(($T2 / 60)) mins $(($T2 % 60)) secs"
T3=$(($T1 + $T2))
echo "total     : $(($T3 / 60)) mins $(($T3 % 60)) secs"
sudo rm $OUTPUT_TAR_GZ.files.list
