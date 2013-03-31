# Build Environment
- you need gcc and nasm
- you need dd and mcopy for generated the raw floppy image

## Mac OS X
- tested in 10.6.8, 10.7.4 & 10.8.2
- install xcode (tested with 4.0.2/4.6) need binutils
- install cross gcc for OSX [Downloads](http://crossgcc.rts-software.org/doku.php)
  - gcc-4.5.2-for-linux32.dmg (65.2 MB)
    
    install to ```/usr/local/gcc-4.5.2-for-linux32/```

  - gcc-4.5.2-for-linux64.dmg (71.7 MB)

    install to ```/usr/local/gcc-4.5.2-for-linux64/```

### install mtools
- download [mtools](http://www.gnu.org/s/mtools/download.html)
- build it with following configuration

  ```
  $ env CFLAGS="-arch i386" LDFLAGS="-arch i386 -liconv" ./configure
  $ make
  $ sudo make install
  ```
### install qemu nasm

  ```
  sudo brew install qemu nasm
  ```
 
## Ubuntu 10, 11.04, 12.04
- the most easy way to setup your environment on ubuntu by excuting script

  ```
  $ sh ./scripts/setup_ubuntu.sh
  ```
- install nasm <br/>
  ```
  $ sudo apt-get install nasm
  ```
  - NASM version 2.08.01 compiled on Jun  5 2010
  - NASM version 2.09.04 compiled on Nov 26 2010

- install qemu

 ```
 $ sudo apt-get install qemu <br/>
 ```
 - tested with qemu version : 0.12.5 - 0.14.0

- install mtools

  ```
  $ sudo apt-get install mtools
  ```
  - (Used to mcopy the MYOS.BIN to floppy image without root permission)

## Fedora 12
- install nasm

  ```
  $ sudo yum install nasm
  ```
 - NASM version 2.07 compiled on Jul 15 2010
- install qemu

  ```
  $ sudo yum install qemu
  ```
 - testied boot successfully with qemu version: 0.11.0
 
## Windows XP sp3 + Cygwin
- install cross gcc for Cygin (build by myself)
  [download](http://julu.myweb.hinet.net/Share/cygwin_toolchain4_bos0821.tgz)
  
   (md5sum: e06bdccb2c44c4cc65097399e8722d95)
- use cygwin setup to install nasm
- download [mtools](http://www.gnu.org/s/mtools/download.html)

  ```
  ./configure <br/>
  make <br/>
  make install <br/>
  ```
  - ```/usr/local/bin/mcopy``` will be used
 
# How To Build

  ```
  $ git clone https://github.com/benwei/bos.git
  $ cd bos
  $ make
  ```
## How to package bos.img
* make package 
* previous command will generate the following image file
  
  a floppy image with vfat-12
  
  ```
  ./bos.img 
  ```

## build all and run
* run with qemu-system-x86_64

  ```
  $ make run
  ```
## for new feature vfs
