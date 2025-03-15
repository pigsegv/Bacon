#!/bin/bash

TEMP_FILE=$(mktemp XXXXX.ld)

START_ADDR=0x7e00
OFFSET=$[$START_ADDR+$(stat --printf="%s" $2)]

echo placing code at $(printf 0x"%x" ${OFFSET})

echo phys = ${OFFSET}";" > $TEMP_FILE
cat $1 >> $TEMP_FILE

ld -T $TEMP_FILE -o $3 $4

rm $TEMP_FILE
