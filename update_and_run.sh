#!/bin/bash
cd "$(dirname "$0")"
wget -q --spider http://google.com

if [ $? -eq 0 ]; then
	git fetch && git pull
	cd build
else
	echo "Offline"
	cd build
fi

sudo ./squiggly
