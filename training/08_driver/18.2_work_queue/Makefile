KERNELDIR ?=/lib/modules/$(shell uname -r)/build
#KERNELDIR ?=~/s5pc100/linux-2.6.35-farsight

123:
	make  -C $(KERNELDIR)  M=$(shell  pwd)  modules
clean:
	#rm -rf *.ko *.o *.mod* modules* Mo* .*.cmd .tmp_versions
	make -C $(KERNELDIR) M=`pwd` clean

obj-m = work_queue.o
