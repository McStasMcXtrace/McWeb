#!/bin/bash
#
# Script to "auto-update" mcweb installations against "current stable"

. /srv/mcweb/mcvenv/bin/activate

cd /srv/mcweb/McWeb

echo "<html>"
echo "<head>"
echo "<title>McWeb update status page</title>"
echo "</head>"
echo "<body>"
echo "<h1>McWeb update process:</h1>"
echo "<pre>"
git status
git pull
git status
echo "</pre>"
echo "<h1>Apply migrations</h1>"
echo "<pre>"
cd /srv/mcweb/McWeb/mcsimrunner/
./manage.py migrate
echo "<h1>Restart uwsgi</h1>"
echo "<pre>"
sudo /etc/init.d/uwsgi_mcweb restart
echo "</pre>"
echo "</body>"
