#KERNELDER ?= /home/linux/linux-2.6.35
KERNELDER ?= /home/linux/linux-2.6.35/
module:
	$(MAKE) -C $(KERNELDER) M=$(shell pwd) modules

clean:
	#rm -rf *.ko *.o *.mod* modules* Mo* .*.cmd .tmp_versions
	make -C $(KERNELDIR) M=`pwd` clean

obj-m=timer.o
