#!/bin/bash

cp open.c /usr/src/minix/servers/vfs/
cp link.c /usr/src/minix/servers/vfs/
cp read.c /usr/src/minix/servers/vfs/
cd /usr/src
make build MKUPDATE=yes >log.txt 2>log.txt
