# Build Environment
- you need gcc and nasm
- you need dd and mcopy for generated the raw floppy image

## Windows XP sp3 + Cygwin
- install cross gcc for Cygin (build by myself)
   http://julu.myweb.hinet.net/Share/cygwin_toolchain4_bos0821.tgz <br/>
   (md5sum: e06bdccb2c44c4cc65097399e8722d95)
- use cygwin setup to install nasm
- download from http://www.gnu.org/s/mtools/download.html
./configure <br/>
make <br/>
make install <br/>
(ps: /usr/local/bin/mcopy will be used)

## Mac OS X (10.6.8)
- install cross gcc for OSX <br/>
   http://crossgcc.rts-software.org/doku.php 
-- gcc-4.5.2-for-linux32.dmg (65.2 MB) <br/>
   install to /usr/local/gcc-4.5.2-for-linux32/ <br/>
-- gcc-4.5.2-for-linux64.dmg (71.7 MB)<br/>
   /usr/local/gcc-4.5.2-for-linux64/<br/>

### install mtools
- download from http://www.gnu.org/s/mtools/download.html
- build it with following configuration
env CFLAGS="-arch i386" LDFLAGS="-arch i386 -liconv" ./configure  <br/>
make <br/>
sudo make install <br/>

### install qemu nasm
sudo brew install qemu nasm

## Ubuntu 10.04, 10.10 and 11.04
- the most easy way to setup your environment on ubuntu by excuting script

  sh ./scripts/setup_ubuntu.sh

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

