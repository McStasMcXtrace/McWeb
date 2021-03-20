#!/bin/bash
set -e

cd /srv/mcweb
source mcvenv/bin/activate

IPADDR=`ip addr show | grep inet\ | cut -f 2 -d\t | cut -f 1 -d/ |grep -v 127 | sed "s/\ //g"`
export IPADDR

#echo -n Please enter your Django upload password and press [ENTER]:
export UPLOADPW=upload

#echo -n Please enter desired simulator MPI cores pr. sim job press [ENTER]:
export MPICORES=1

sed "s/dc=risoe,dc=dk/${LDAPDOMAIN}/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py.in > /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py
sed -i "s/@LDAP_PW@/${LDAP_PASS}/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py
sed -i "s/@IPADDR@/127.0.0.1/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py
sed -i "s/@SERVERNAME@/localhost/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py
sed -i "s/@UPLOADPW@/${UPLOADPW}/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py
sed -i "s/@MPICORES@/${MPICORES}/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py
sed -i "s/'django_auth_ldap/\#'django_auth_ldap/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py
sed -i "s/\#'django.contrib.auth/'django.contrib.auth/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py
#sed -i "s/DEBUG = True/DEBUG = False/" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py
sed -i "s/-dev//g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py

cd McWeb/mcsimrunner/
python manage.py migrate
python manage.py collect_instr
echo Please assist Django in creation of your djangoadmin user:
python manage.py createsuperuser --username=djangoadmin --email=admin@localhost
echo
echo Essential setup done, here is a summary:
echo
echo Django setup:
echo username: djangoadmin
echo "password: (as just entered)"
echo email-adress: admin@localhost
echo Django upload pass: $UPLOADPW
echo

