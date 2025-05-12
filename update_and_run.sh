#!/bin/bash

wget -q --spider http://google.com

if [ $? -eq 0 ]; then
	git fetch && git pull
	cd build
	make
else
	echo "Offline"
	cd build
fi

sudo squiggly
