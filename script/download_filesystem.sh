#!/bin/bash

echo $(ps aux | grep idf.py | grep monitor | awk '{print $2}')
IDS=$(ps aux | grep idf.py | grep monitor | awk '{print $2}')

for PROCESS in $IDS; do
    kill $PROCESS
done

esptool.py -p /dev/ttyUSB1 -b 230400 read_flash 0x00310000 0x0004b000 filesystem_content.bin

rmdir ./embedded_flash

./script/mkspiffs -u ./embedded_flash filesystem_content.bin

rm filesystem_content.bin
