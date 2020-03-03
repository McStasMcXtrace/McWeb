#!/bin/sh

STARTDIR=`pwd`
# Basic infrastructure
apt-get -y install net-tools iproute2 sudo openssh-server xbase-clients zip unzip

# Add nonfree and contrib repo
sed -i.bak s/main/main\ contrib\ non-free/g /etc/apt/sources.list
apt-get update

# Add mccode repo
cd /etc/apt/sources.list.d/
rm mccode.list
wget http://packages.mccode.org/debian/mccode.list
apt-get update
cd $STARTDIR

# Base packages for McCode + MPI
apt-get -y install mcstas-suite-perl mcstas-suite-python mcxtrace-suite-perl mcxtrace-suite-python openmpi-bin libopenmpi-dev
# Ensure we use mcdoc.pl rather than python version
ln -sf /usr/share/mcstas/2.6/bin/mcdoc.pl /usr/bin/mcdoc
# Ensure mcplot.pl uses "proper" PGPLOT rather than GIZA
cd /usr/lib/x86_64-linux-gnu
sudo ln -sf ../libpgplot.so libpgplot.so.0
sudo ln -sf ../libcpgplot.so libcpgplot.so.0
cd $STARTDIR

# Remove stop apache2 from being default webserver
update-rc.d apache2 remove
service apache2 stop

# Packages for bootstrapping an McWeb instance
apt-get -y install git libsasl2-dev python-dev libldap2-dev libssl-dev python-virtualenv makepasswd nginx php-fpm php-mysql php-xml php-curl php-zip php-gd php-mbstring php-xmlrpc php-soap php-intl php-ldap

rm -rf /srv/mcweb
mkdir /srv/mcweb
sudo chown -R www-data:www-data /srv/mcweb /var/www/ 

# Bootstrap McWeb via sudo / git
cd /srv/mcweb
sudo -H -u www-data virtualenv mcvenv
sudo -u www-data cp mcvenv/bin/activate mcvenv_finishup
echo pip install -I Django==1.8.2 django-auth-ldap==1.2.7 simplejson python-ldap >> mcvenv_finishup
#echo pip install -I django-auth-ldap==1.2.7 >> mcvenv_finishup
echo pip install uwsgi  >> mcvenv_finishup
sudo -H -u www-data  bash mcvenv_finishup

sudo -H -u www-data  git clone https://github.com/McStasMcXtrace/McWeb

# Pick and pull the STABLE branch
cd McWeb
sudo -H -u www-data  git checkout MCWEB_STABLE_2.0
sudo -H -u www-data  git pull

cd /srv/mcweb
ln -sf /srv/mcweb/McWeb/scripts/uwsgi_mcweb /etc/init.d/uwsgi_mcweb
update-rc.d uwsgi_mcweb defaults

IPADDR=`ip addr show | grep inet\ | cut -f 2 -d\t | cut -f 1 -d/ |grep -v 127 | sed "s/\ //g"`
SERVERNAME=`hostname`
export IPADDR
export SERVERNAME

echo -n Please enter your Django upload password and press [ENTER]:
read UPLOADPW
export UPLOADPW

echo -n Please enter desired simulator MPI cores pr. sim job press [ENTER]:
read MPICORES
export MPICORES

# Allow www-data to restart uwsgi
echo >> /etc/sudoers
echo "# Allow www-data to restart uwsgi_mcweb service" >> /etc/sudoers
echo "www-data ALL = NOPASSWD: /etc/init.d/uwsgi_mcweb" >> /etc/sudoers

# Last setup of uwsgi etc
echo Resuming setup...
sed "s/dc=risoe,dc=dk/${LDAPDOMAIN}/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py.in > /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py
sed -i "s/@LDAP_PW@/${LDAP_PASS}/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py 
sed -i "s/@IPADDR@/127.0.0.1/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py 
sed -i "s/@SERVERNAME@/localhost/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py
sed -i "s/@UPLOADPW@/${UPLOADPW}/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py
sed -i "s/@MPICORES@/${MPICORES}/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py
sed -i "s/'django_auth_ldap/\#'django_auth_ldap/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py
sed -i "s/\#'django.contrib.auth/'django.contrib.auth/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py
sed -i "s/DEBUG = True/DEBUG = False/" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py
sed -i "s/-dev//g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py

# Simple, static "admin" landing page
cd /srv/mcweb
sudo -u www-data mkdir landing
sed "s/@HOSTNAME@/${SERVERNAME}/g" McWeb/landingpage/landingpage.in.html > landing/index.html
chown www-data:www-data landing/index.html

cd /srv/mcweb
sudo -u www-data mkdir McWeb/mcsimrunner/sim/intro-ns
sudo -u www-data cp /usr/share/mcstas/2.6/examples/templateSANS.instr /srv/mcweb/McWeb/mcsimrunner/sim/intro-ns/SANSsimple.instr
sudo -u www-data cp mcvenv/bin/activate McWeb_finishup
echo cd McWeb/mcsimrunner/ >> McWeb_finishup
echo python manage.py migrate >> McWeb_finishup
echo python manage.py collect_instr >>  McWeb_finishup
echo echo Please assist Django in creation of your djangoadmin user: >>  McWeb_finishup
echo python manage.py createsuperuser --username=djangoadmin --email=admin@localhost >>  McWeb_finishup >>  McWeb_finishup
echo echo -n Please enter your Django admin user pass again and press \[ENTER\]: >>  McWeb_finishup
echo read DJANGO_PASS >>  McWeb_finishup
echo echo >>  McWeb_finishup
echo echo Essential setup done, here is a summary: >>  McWeb_finishup
echo echo >>  McWeb_finishup
echo echo Django setup: >>  McWeb_finishup
echo echo username: djangoadmin >>  McWeb_finishup
echo echo password: \$DJANGO_PASS >>  McWeb_finishup
echo echo email-adress: admin@localhost >>  McWeb_finishup
echo echo Django upload pass: $UPLOADPASS >>  McWeb_finishup
echo echo >>  McWeb_finishup 
echo crontab /srv/mcweb/McWeb/scripts/cronjobs.txt >> McWeb_finishup 

sudo -H -u www-data  bash McWeb_finishup
/etc/init.d/uwsgi_mcweb start

cat /srv/mcweb/McWeb/scripts/nginx-default > /etc/nginx/sites-enabled/default
service nginx restart


