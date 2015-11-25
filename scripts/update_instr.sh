#!/bin/bash
cd /srv/mcweb/McWeb

OLD=$(cat checksum)

find sim/ -type f -exec md5sum {} \; | sort -k 34 | md5sum | cut -d' ' -f1 > newchecksum
NEW=$(cat newchecksum)
rm newchecksum

if [ $OLD == $NEW ]; then
	echo sim folder checksum has not changed, exiting.
	echo
	exit
elif [ $OLD != $NEW ]; then
	echo sim folder checksum has changed, running updatesimulations.sh ...
	echo
	scripts/__compile_instr.sh
	echo updating django instrument db ...
	echo
	python manage.py populate_db
	echo creating new checksum file...
	rm checksum
	find sim/ -type f -exec md5sum {} \; | sort -k 34 | md5sum | cut -d' ' -f1 > checksum
	cat checksum
fi

