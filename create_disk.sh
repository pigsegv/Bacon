#!/bin/bash

PARTITION_SIZE_GiB=2
PARTITION_OFFSET_SECTORS=64

./bin/format-mbr $1 $2
dd if=/dev/zero of=$3 bs=4096 count=$[PARTITION_SIZE_GiB * 1024 * 1024 * 1024] iflag=count_bytes
mkfs.ext2 $3
dd seek=$[$PARTITION_OFFSET_SECTORS * 512] count=$(stat --printf="%s" $3) oflag=seek_bytes iflag=count_bytes bs=4096 < $3 1<> $2

