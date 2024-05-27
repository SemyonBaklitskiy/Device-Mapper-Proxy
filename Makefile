obj-m := dmp.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules -j$(shell nproc)

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean -j$(shell nproc)