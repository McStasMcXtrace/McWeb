#!/bin/sh
#
# Script to backup all parts of an "e-neutrons" style mediawiki database and configuration:
#
# 1) Image files in /srv/mcweb/mediawiki/images
# 2) Logo files in /srv/mcweb/mediawiki/logos
# 3) Extensions in /srv/mcweb/mediawiki/extensions
# 4) PostgreSQL database given as $1 (on legacy e-neutrons.org site named vntwiki)
# 5) LocalSettings.php (DB passwords stripped out for security)

# Check if database name given, otherwise use "vntwiki"
DBNAME=$1

if [ "x$1" = "x" ]; then
    # No arguments
    DBNAME="vntwiki"
    echo No datatabase name given - default $DBNAME will be applied!
fi
MWDIR="/srv/mcweb/mediawiki/"
BACKUPBASE="mediawiki_"

MONTH=`date +"%m"`
DAY=`date +"%d"`
YEAR=`date +"%Y"`
BACKUPDIR="${BACKUPBASE}${YEAR}${MONTH}${DAY}"
IMAGES="${BACKUPDIR}/images.tgz"
LOGOS="${BACKUPDIR}/logos.tgz"
EXTS="${BACKUPDIR}/extensions.tgz"
DBDUMP="${BACKUPDIR}/pgdump.sql"

WORKDIR=`pwd`

mkdir $BACKUPDIR
cd $MWDIR
echo Generating image archive
tar cfz $WORKDIR/$IMAGES images
echo Generating logo archive
tar cfz $WORKDIR/$LOGOS logos
echo Generating extension archive
tar cfz $WORKDIR/$EXTS extensions
echo Dumping database $DBNAME
sudo -u postgres -H -- pg_dump $DBNAME > $WORKDIR/$DBDUMP
cp $MWDIR/LocalSettings.php $WORKDIR/$BACKUPDIR/LocalSettings.php
cd $WORKDIR/$BACKUPDIR
echo Storing/anonymising LocalSettings.php
# MediaWiki / Database config
sed -i 's/\$wgSitename = "\(.*\)";/\$wgSitename = "@SITENAME@";/' LocalSettings.php
sed -i 's/\$wgServer = "\(.*\)";/\$wgServer = "@SERVERNAME@";/' LocalSettings.php
sed -i 's/\$wgEmergencyContact = "\(.*\)";/\$wgEmergencyContact = "@ADMINMAIL@";/' LocalSettings.php
sed -i 's/\$wgPasswordSender = "\(.*\)";/\$wgPasswordSender = "@ADMINMAIL@";/' LocalSettings.php
sed -i 's/\$wgDBname = "\(.*\)";/\$wgDBname = "@DBNAME@";/' LocalSettings.php
sed -i 's/\$wgDBuser = "\(.*\)";/\$wgDBuser = "@DBUSER@";/' LocalSettings.php
sed -i 's/\$wgDBpassword = "\(.*\)";/\$wgDBpassword = "@DBPASS@";/' LocalSettings.php
# LDAP config
sed -i 's/\$wgLDAPDomainNames = array(\"\(.*\)\");/\$wgLDAPDomainNames = array(\"@LDAP_FULLDOM_UNDERSCORE@\");/' LocalSettings.php
sed -i 's/\$wgLDAPSearchAttributes = array( \"\(.*\)\" => "uid" );/\$wgLDAPSearchAttributes = array( \"@LDAP_FULLDOM_UNDERSCORE@\" => "uid" );/' LocalSettings.php
sed -i 's/\$wgLDAPServerNames = array(\"\(.*\)\" => "localhost");/\$wgLDAPServerNames = array(\"@LDAP_FULLDOM_UNDERSCORE@\" => "localhost");/' LocalSettings.php
sed -i 's/\$wgLDAPEncryptionType = array(\"\(.*\)\" => "clear");/\$wgLDAPEncryptionType = array(\"@LDAP_FULLDOM_UNDERSCORE@\" => "clear");/' LocalSettings.php
sed -i 's/\$wgLDAPPort = array(\"\(.*\)\" => 389);/\$wgLDAPPort = array(\"@LDAP_FULLDOM_UNDERSCORE@\" => 389);/' LocalSettings.php
sed -i 's/\$wgLDAPBaseDNs = array(\"\(.*\)\" => \"\(.*\)\");/\$wgLDAPBaseDNs = array(\"@LDAP_FULLDOM_UNDERSCORE@\" => \"@LDAP_DN@\");/' LocalSettings.php
sed -i 's/\$wgLDAPGroupBaseDNs = array(\"\(.*\)\" => \"\(.*\)\");/\$wgLDAPGroupBaseDNs = array(\"@LDAP_FULLDOM_UNDERSCORE@\" => \"@LDAP_GROUP_OU@\");/' LocalSettings.php
# PIWIK
sed -i 's/\$wgPiwikURL = "\(.*\)";/\$wgPiwikURL = "@PIWIK_URL@";/' LocalSettings.php
