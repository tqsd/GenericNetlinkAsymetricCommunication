#!/usr/bin/env bash

# clear kernel log
sudo rmmod qndkm.ko
sudo dmesg -c > /dev/null

make clean
make
sudo insmod qndkm.ko
echo "inserted qndkm.ko"
