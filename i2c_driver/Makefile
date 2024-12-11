# Makefile
obj-m += i2c_driver.o

all:
#native build
#	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

#use external headers and cross compile
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C /home/arun/projects/arun/source/linux-rpi-5.15.y M=$(PWD) modules

clean:
#native build
#	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

#use external headers and cross compile clean
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C /home/arun/projects/arun/source/linux-rpi-5.15.y M=$(PWD) clean

