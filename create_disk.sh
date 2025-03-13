#!/bin/bash

PARTITION_SIZE_GiB=2
PARTITION_OFFSET_SECTORS=64

./bin/format-mbr $3 $1

if [[ $? -ne 0 ]] ; then
	exit 1
fi

dd if=/dev/zero of=$2 bs=4096 count=$[PARTITION_SIZE_GiB * 1024 * 1024 * 1024] iflag=count_bytes
mkfs.ext2 $2

dd seek=$[$PARTITION_OFFSET_SECTORS * 512] count=$(stat --printf="%s" $2)\
	oflag=seek_bytes iflag=count_bytes bs=4096 < $2 1<> $1

dd seek=512 count=$(stat --printf="%s" $4)\
	oflag=seek_bytes iflag=count_bytes bs=4096 < $4 1<> $1

rm $3

