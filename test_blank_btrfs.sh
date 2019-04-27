#!/bin/bash

set -eu

uname -a
IMG=$(mktemp)
MNT=$IMG.mnt
mkdir $MNT
echo "allocating 1GB at $IMG, formatting as btrfs"
fallocate -l 1G $IMG
mkfs.btrfs -q $IMG
echo "mounting to $MNT"
sudo mount $IMG $MNT
sudo chmod 777 $MNT
MYDIR=$PWD
cd $MNT
$MYDIR/fallocate_write || true
echo "cleaning up"
cd $MYDIR
sudo umount $MNT
rm $IMG
