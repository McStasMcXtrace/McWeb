#!/bin/bash

cd /srv/mcweb/McWeb/mcsimrunner/

OLD=$(cat checksum)

find sim/ -type f -exec md5sum {} \; | sort -k 34 | md5sum | cut -d' ' -f1 > newchecksum
NEW=$(cat newchecksum)
rm newchecksum

export PATH=/usr/lib64/openmpi/bin/mpicc:/usr/local/mcstas/2.6/bin:/usr/local/sbin:/sbin:/bin:/usr/sbin:/usr/bin:/opt/puppetlabs/bin:/root/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/pgplot

if [ "$OLD" == $NEW ]; then
	echo sim folder checksum has not changed, exiting.
	echo
	exit
elif [ "$OLD" != $NEW ]; then
	echo sim folder checksum has changed, running collectinstr \(output sent to url /static/compile_status.html\) ...
	echo
	python manage.py collect_instr > /srv/mcweb/McWeb/mcsimrunner/static/compile_status.html
	echo creating new checksum file...
	rm checksum
	find sim/ -type f -exec md5sum {} \; | sort -k 34 | md5sum | cut -d' ' -f1 > checksum
	cat checksum
fi
