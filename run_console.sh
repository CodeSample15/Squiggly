#!/bin/bash

wget -q --spider http://google.com

if [ $? -eq 0 ]; then
	cd Squiggly && git fetch && git pull
	cd build
	make
else
	echo "Offline"
fi

sudo ~/Documents/Projects/Squiggly/build/squiggly
