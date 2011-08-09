#!/bin/sh
IMG=floppy.img

domount()
{
sudo mount -t vfat -o rw -o loop $IMG fda
}

doumount()
{
sudo umount fda
}

# main
case $1 in
u*)
doumount
;;
m*)
ret=`mount | grep fda`
if [ "$ret" = "" ]; then
	domount
echo "mounted successfully"
else
echo "has been mounted successfully"
	doumount
	domount
echo "has been remounted successfully"
fi
;;
esac

