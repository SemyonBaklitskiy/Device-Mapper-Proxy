# Device mapper proxy
## Technical requirements (Linux)
1. Ubuntu 22.04.4 LTS
2. Linux Kernel version 6.2 and above
3. Compiler gcc-11 (gcc version 11.4.0) or gcc-12 (gcc version 12.3.0), better use gcc-12
4. GNU Make 4.3

## Building and loading module
1. Clone repository into the directory

```
git clone https://github.com/SemyonBaklitskiy/Device-Mapper-Proxy
```
2. Change directory to Device-Mapper-Proxy
```
cd Device-Mapper-Proxy
```
3. Add executuon permissions for build_and_load.sh script

```
sudo chmod +x build_and_load.sh
```

4. Run script giving the compiler as an argument

```
./build_and_load.sh gcc-12
```

Success building and loading:
```
Loading module (maybe password required):
dmp                    16384  0
```

## Creating block devices and testing module
1. Add execution permissions for test.sh script

```
sudo chmod +x test.sh
```

2. Run script giving the size of block devices (kb) as an argument

```
./test.sh 16
```

Example of stats:

```
Read:
    reqs: 13
    avg size: 4096
Write:
    reqs: 5
    avg size: 4096
Total:
    reqs: 18
    avg size: 4096
```

## Removing block devices and unloading module
1. Add execution permissions for clean.sh script

```
sudo chmod +x clean.sh
```

2. Run script

```
./clean.sh
```
