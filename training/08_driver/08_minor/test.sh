#!/bin/bash

MODULE=minor

echo "====== compile driver and test app ======"
make
gcc test.c

sudo dmesg  -C
echo -e "\n====== ls /dev/xhello-* ======"
ls /dev/xhello-*

echo -e "\n====== sudo insmod $MODULE.ko ======"
sudo insmod $MODULE.ko
echo "ls /dev/xhello-*"
ls /dev/xhello-*

echo -e "\n====== ./a.out ======"
sudo ./a.out /dev/xhello-0
sudo ./a.out /dev/xhello-1

echo -e "\n====== sudo rmmod $MODULE ======"
sudo rmmod $MODULE
dmesg
echo -e "\nls /dev/xhello-*"
ls /dev/xhello-*

echo -e "\n====== make clean ======"
make clean
rm a.out
