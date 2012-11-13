all: pi qemu

pi:
	make -f make.pi

qemu:
	make -f make.qemu
