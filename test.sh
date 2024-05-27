#!/bin/bash

echo "Given parameters:"
echo "size=$1"

echo "Creating block devices (maybe password required):"
sudo dmsetup create zero1 --table "0 $1 zero"
sudo dmsetup create dmp1 --table "0 $1 dmp /dev/mapper/zero1"
ls -l /dev/mapper
lsblk

echo "Testing (maybe password required):"

sudo dd if=/dev/random of=/dev/mapper/dmp1 bs=4k count=1
sudo dd if=/dev/random of=/dev/mapper/dmp1 bs=4k count=2
sudo dd if=/dev/random of=/dev/mapper/dmp1 bs=8k count=1

sudo dd of=/dev/null if=/dev/mapper/dmp1 bs=4k count=1
sudo dd of=/dev/null if=/dev/mapper/dmp1 bs=4k count=2
sudo dd of=/dev/null if=/dev/mapper/dmp1 bs=8k count=1

sudo cat /sys/block/dm-0/volumes