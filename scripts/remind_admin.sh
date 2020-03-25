#!/bin/bash
source /srv/mcweb/mcvenv/bin/activate
cd /srv/mcweb/McWeb/mcsimrunner/
export PATH=/usr/local/sbin:/sbin:/bin:/usr/sbin:/usr/bin:/opt/puppetlabs/bin:$PATH
python manage.py remind_admin
