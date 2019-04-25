#!/bin/bash

set -eu

uname -a
IMG=$(mktemp)
MNT=$IMG.mnt
mkdir $MNT
fallocate -l 1G $IMG
mkfs.btrfs -q $IMG
sudo mount $IMG $MNT
echo "mounted at $MNT"
sudo chmod 777 $MNT
MYDIR=$PWD
cd $MNT
$MYDIR/fallocate_write || true
cd $MYDIR
sudo umount $MNT
rm $IMG
