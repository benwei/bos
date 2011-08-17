# Build Environment
- you need gcc in basic setup
- you need dd for generated the raw floppy image

## Ubuntu 10.04, 10.10 and 11.04
- install nasm <br/>

 $ sudo apt-get install nasm <br/>
 (NASM version 2.08.01 compiled on Jun  5 2010) <br/>
 (NASM version 2.09.04 compiled on Nov 26 2010)

- install qemu <br/>
 $ sudo apt-get install qemu <br/>
 (tested with qemu version : 0.12.5 - 0.14.0)

- install mtools <br/>
 $ sudo apt-get install mtools
 (Used to mcopy the MYOS.BIN to floppy image without root permission)

## Fedora 12
- install nasm <br/>
 $ sudo yum install nasm <br/>
 (NASM version 2.07 compiled on Jul 15 2010)

- install qemu <br/>
 $ sudo yum install qemu <br/>
  (testied boot successfully with qemu version: 0.11.0)
 
# How To Build
  make <br/>

## How to package bos.img
  - make package 
  - previous command generated image file <br/>
  -- ./bos.img # a floppy image with vfat-12

## build all and run
  - make run # run with qemu-system-x86_64

