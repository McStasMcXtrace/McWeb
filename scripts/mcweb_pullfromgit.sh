#!/bin/bash
#
# Script to "auto-update" mcweb installations against "current stable"

cd /srv/mcweb/McWeb

echo "<html>"
echo "<head>"
echo "<title>McWeb update status page</title>"
echo "</head>"
echo "<body>"
echo "<h1>McWeb update process:</h1>"
echo "<pre>"
git checkout MCWEB_STABLE_2.0
git pull
git status
echo "</pre>"
echo "<h1>Apply migrations</h1>"
echo "<pre>"
cd /srv/mcweb/McWeb/mcsimrunner/
./manage.py migrate
echo "</pre>"
echo "</body>"