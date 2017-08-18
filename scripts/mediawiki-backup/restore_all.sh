#!/bin/sh

WORKDIR=`pwd`

# Check if we were givern a mediawiki archive as a $1
if [ -n "$1" ]; then
    echo Attempting to restore mediawiki archive from $1
    if [ -d "$1" ]; then
	echo Proceeding with $1
	if [ -f "$WORKDIR/$1/images.tgz" ]; then
	    echo Unpacking image archive
	    cd /srv/mcweb/mediawiki
	    tar xzf $WORKDIR/$1/images.tgz
	else
	    echo $1/images.tgz did not exist, dropping image restore
	fi
        if [ -f "$WORKDIR/$1/logos.tgz" ]; then
	    echo Unpacking logo archive
	    cd /srv/mcweb/mediawiki
	    tar xzf $WORKDIR/$1/logos.tgz
	else
	    echo $1/logos.tgz did not exist, dropping logo restore
	fi
	if [ -f "$WORKDIR/$1/extensions.tgz" ]; then
	    echo Unpacking extension archive
	    cd /srv/mcweb/mediawiki
	    tar xzf $WORKDIR/$1/extensions.tgz
	else
	    echo $1/extensions.tgz did not exist, dropping extension restore
	fi
	if [ -f "$WORKDIR/$1/pgdump.sql" ]; then
	    echo Installing database
	    cd /srv/mcweb/
	    sudo -u postgres -H -- psql my_wiki < $WORKDIR/$1/pgdump.sql
	else
	    echo $1/pgdump.sql did not exist, dropping database restore
	fi
	
    else
	echo Directory $1 nonexistent, dropping restore
    fi
fi
