#!/bin/bash
# this script should be run once every night to send a notification to the admin in there are new signups
if [ -f /srv/mcweb/McWeb/mcsimrunner_ng/new_signups.csv ]; then
    YEARDATE="$(python /srv/mcweb/McWeb/scripts/_gettodaystring.py)"
    echo "moving new_signups.csv to /srv/mcweb/McWeb/useradmin/new_${YEARDATE}.csv ..."
    mv /srv/mcweb/McWeb/mcsimrunner_ng/new_signups.csv /srv/mcweb/McWeb/useradmin/new_"${YEARDATE}".csv
    echo "sending email to admin with instructions ..."
    mailx -s "mcweb subscriptions: new_${YEARDATE}.csv" jaga@fysik.dtu.dk,pkwi@fysik.dtu.dk < /srv/mcweb/McWeb/scripts/_adminemail
else
    echo "no new signups, exiting (file new_signups.csv does not exist)"
fi
