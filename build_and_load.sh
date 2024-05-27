#!/bin/bash

echo "Given parameters:"
echo "compiler=$1"

echo "Building module!"
make CC=$1 HOSTCC=$1

echo "Loading module (maybe password required):"
sudo insmod dmp.ko
lsmod | grep dmp
