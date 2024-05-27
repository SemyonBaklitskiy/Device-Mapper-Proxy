#!/bin/bash

echo "Removing block devices (maybe password required):"
sudo dmsetup remove dmp1
sudo dmsetup remove zero1
ls -l /dev/mapper

echo "Unloading module (maybe passsword required)"
sudo rmmod dmp
lsmod | grep dmp

echo "Cleaning directory!"
make clean

#sudo dmesg #if you want to see kernel messages
