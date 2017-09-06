#!/bin/bash
#
# Script to "auto-update" moodle installations against whatever latest (chosen) release Moodle

cd /srv/mcweb/moodle
git checkout `cat ../McWeb/scripts/MOODLE_PREFERRED_VERSION `
git pull

cd /srv/mcweb/moosh
git pull
