#!/bin/sh
#
# Script for doing restores of moodle courses
#

RESTOREJOBS="/srv/mcweb/moodle-restore-jobs"
MOODLEDIR="/srv/mcweb/moodle"

# Look for restore files
for RESTORE in `ls $RESTOREJOBS/ | grep -v \\.lock`
do
	echo $RESTOREJOBS/$RESTORE
        if [ -f $RESTOREJOBS/$RESTORE ]; then
                if [ ! -f $RESTOREJOBS/$RESTORE.lock ]; then
			touch $RESTOREJOBS/$RESTORE.lock
                        cd $MOODLEDIR
			pwd
                        sh $RESTOREJOBS/$RESTORE
                        rm $RESTOREJOBS/$RESTORE
                        rm $RESTOREJOBS/$RESTORE.lock
                fi
        fi
done
