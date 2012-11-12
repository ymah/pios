#!/bin/sh

debugFlag=$1

if [ x"$debugFlag" = x"--debug" ]
then
	qemuOpts="-s -S -D qemu.log -d exec,ioport,unimp"
else
#	qemuOpts="-S -serial none -monitor stdio"
	qemuOpts=""
fi
qemu-system-arm -M versatilepb $qemuOpts -kernel kernel-qemu.img
