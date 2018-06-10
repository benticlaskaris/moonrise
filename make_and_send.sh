#!/bin/sh

if [ $# -eq 0 ]; then
	echo "Usage : ./make_and_send.sh <ip address>"
else
	make && nxlink -a $1 moonrise.nro
fi