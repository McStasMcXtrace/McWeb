#!/bin/bash
#
# Backup script for moodle including:
# 1) Full moodle database (pulled from moodle config)
# 2) Moodledata folder
# 3) Moodle folder
# 4) mbz course dumps if they exist
#
# Intended for weekly run through cron and should only keep the last backup from last month
#

export PATH=/usr/local/sbin:/sbin:/bin:/usr/sbin:/usr/bin:/opt/puppetlabs/bin:$PATH

# Find out data info
YEAR=`date +'%Y'`
MONTH=`date +'%m'`
DAY=`date +'%d'`
LASTMONTH=`date +'%m' --date="$(date +%Y-%m-15) -1 month"`
LASTMONTHYEAR=`date +'%Y' --date="$(date +%Y-%m-15) -1 month"`

# Where moodle is, where backups are to be stored etc.
MOODLEDIR="/srv/mcweb/moodle"
MBZDIR="/srv/mcweb/moodle-course-templates"
MOODLEDATA="/srv/moodledata"
MOOSHDIR="/srv/mcweb/moosh"
BACKUPDIR="/srv/mcweb/moodle-fullbackups"
echo \<html\>
echo \<head\>
echo \</head\>
echo \<body\>
echo \<h1\>FULL moodle backup set being created $YEAR / $MONTH / $DAY\</h1\>
echo \<pre\>

cd $MOODLEDIR
DBUSER=`grep dbuser config.php | cut -f2 -d\'`
DBPASS=`grep dbpass config.php | cut -f2 -d\'`
DBNAME=`grep dbname config.php | cut -f2 -d\'`
# Maintenance mode for moodle
php admin/cli/maintenance.php --enable


# Create relevant folder structure
mkdir -p $BACKUPDIR
mkdir -p $BACKUPDIR/$YEAR/
mkdir -p $BACKUPDIR/$YEAR/$MONTH
mkdir -p $BACKUPDIR/$YEAR/$MONTH/$DAY

# Start the work
cd $BACKUPDIR/$YEAR/$MONTH/$DAY

# MySQL dump
echo Performing MySQL dump
mysqldump -u$DBUSER -p$DBPASS $DBNAME > ${DBNAME}_${YEAR}_${MONTH}_${DAY}.sql

# Moodle archives
echo Performing backup of moodle install
cd $MOODLEDIR/..
tar cfz $BACKUPDIR/$YEAR/$MONTH/$DAY/moodle.tgz `basename $MOODLEDIR`
echo Performing backup of moodle course templates
cd $MBZDIR/..
tar cfz $BACKUPDIR/$YEAR/$MONTH/$DAY/moodle-course-templates.tgz `basename $MBZDIR`
echo Performing backup of moodledata
cd $MOODLEDATA/..
tar cfz $BACKUPDIR/$YEAR/$MONTH/$DAY/moodledata.tgz `basename $MOODLEDATA`
echo Performing backup of moosh
cd $MOOSHDIR/..
tar cfz $BACKUPDIR/$YEAR/$MONTH/$DAY/moosh.tgz `basename $MOOSHDIR`

# Maintenance mode for moodle off again
cd $MOODLEDIR
php admin/cli/maintenance.php --disable

# Create an archive in the $MONTH location
cd $BACKUPDIR/$YEAR/$MONTH
tar cfz moodle_fullbackup_${YEAR}_${MONTH}_${DAY}.tgz ${DAY}

echo Final backupfile is moodle_fullbackup_${YEAR}_${MONTH}_${DAY}.tgz :
du -sh moodle_fullbackup_${YEAR}_${MONTH}_${DAY}.tgz

# Cleanup
rm -rf $DAY

if [ -d $BACKUPDIR/$LASTMONTHYEAR/$LASTMONTH ]; then
    cd $BACKUPDIR/$LASTMONTHYEAR/$LASTMONTH
    echo Cleaning up so that only last arch from previous month is available:
    NUMOLDBACKUPS=`ls -rt *tgz|wc -l`
    echo Found $NUMOLDBACKUPS
    echo - will save:
    echo `ls -rt *tgz|tail -1`
    if (( $NUMOLDBACKUPS > "1" )); then
	REMOVE=`ls -rt *tgz|head -$(($NUMOLDBACKUPS-1))`
	echo ... and remove
	echo $REMOVE | xargs -n1 echo
	echo $REMOVE | xargs -n1 rm
    else
	echo ... which was the only backup
    fi
else
    echo No backups from last month it seems...
fi
echo \</pre\>
echo \</body\>
echo \</html\>
