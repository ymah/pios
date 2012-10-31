#!/bin/sh

debugFlag=$1

if [ x"$debugFlag" = x"--debug" ]
then
	qemuOpts="-s -S"
else
	qemuOpts="-S -serial none -monitor stdio"
fi
qemu-system-arm -M versatilepb $qemuOpts -kernel kernel-qemu.img
