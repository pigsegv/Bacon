#!/bin/bash

PARTITION_SIZE_GiB=2
PARTITION_OFFSET_SECTORS=64

DISK_NAME=$2
BOOTSECTOR=$3
STAGE15=$4

if [[ $1 = "format" ]] ; then
	touch ${DISK_NAME}
	TEMP_FILE=$(mktemp XXXXX.img)

	if [[ $? -ne 0 ]] ; then
		exit 1
	fi

	dd if=/dev/zero of=${TEMP_FILE} bs=4096 count=$[PARTITION_SIZE_GiB * 1024 * 1024 * 1024] iflag=count_bytes status=none
	mkfs.ext2 -q ${TEMP_FILE}


	dd seek=$[$PARTITION_OFFSET_SECTORS * 512] count=$(stat --printf="%s" ${TEMP_FILE})\
		oflag=seek_bytes iflag=count_bytes bs=4096 status=none < ${TEMP_FILE} 1<> ${DISK_NAME}

	rm ${TEMP_FILE}

	exit 0
fi


./bin/format-mbr ${BOOTSECTOR} ${DISK_NAME}

if [[ $? -ne 0 ]] ; then
	exit 1
fi


dd seek=512 count=$[(PARTITION_OFFSET_SECTORS - 1) * 512]\
	oflag=seek_bytes iflag=count_bytes bs=4096 status=none < /dev/zero 1<> ${DISK_NAME}

dd seek=512 count=$(stat --printf="%s" ${STAGE15})\
	oflag=seek_bytes iflag=count_bytes bs=4096 status=none < ${STAGE15} 1<> ${DISK_NAME}

