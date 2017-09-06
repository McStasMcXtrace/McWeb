#!/bin/bash
#
# Script to "auto-update" moodle installations against whatever latest (chosen) release Moodle

cd /srv/mcweb/moodle

echo "<html>"
echo "<head>"
echo "<title>Moodle update status page</title>"
echo "</head>"
echo "<body>"
echo "<h1>Moodle update process:</h1>"
git checkout `cat ../McWeb/scripts/MOODLE_PREFERRED_VERSION `
git pull
git status
echo "<h1>Moosh update process:</h1>"
cd /srv/mcweb/moosh
git pull
git status
echo "</body>"