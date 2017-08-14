#!/bin/sh

STARTDIR=`pwd`
# Basic infrastructure
apt-get install net-tools sudo openssh-server xbase-clients

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
apt-get install mcstas-suite-perl mcstas-suite-python openmpi-bin libopenmpi-dev

# Packages for bootstrapping an McWeb instance
apt-get install git libsasl2-dev python-dev libldap2-dev libssl-dev python-virtualenv makepasswd nginx mysql-server php-fpm php-mysql

# Install nginx and remove stop apache2 from being default webserver
update-rc.d apache2 remove
service apache2 stop
apt-get install nginx

rm -rf /srv/mcweb
mkdir /srv/mcweb
sudo chown -R www-data:www-data /srv/mcweb /var/www/

# Bootstrap McWeb via sudo / git
cd /srv/mcweb
sudo -u www-data -H virtualenv mcvenv
echo pip install -I Django==1.8.2 >> mcvenv_finishup
echo pip install simplejson django_auth_ldap uwsgi python-ldap >> mcvenv_finishup

sudo -u www-data git clone https://github.com/McStasMcXtrace/McWeb
echo cd McWeb/mcsimrunner/ >> McWeb_finishup
echo python manage.py migrate >> McWeb_finishup
echo python manage.py collect_instr >>  McWeb_finishup
echo echo Please assist Django in creating a local superuser account: >>  McWeb_finishup
echo echo python manage.py createsuperuser >>  McWeb_finishup
echo python manage.py createsuperuser >>  McWeb_finishup
echo echo Please run the following commands as root to set up your Django with LDAP: >>  McWeb_finishup
echo echo \\\> python ldap_initdb.py USE_YOUR_LDAP_PASSWD_HERE >>  McWeb_finishup
echo echo >>  McWeb_finishup
echo echo Afterwards, please sudo /etc/init.d/uwsgi_mcweb start >>  McWeb_finishup

# Moodle
sudo -u www-data git clone https://github.com/moodle/moodle.git
echo cd moodle >> moodle_finishup
echo git checkout MOODLE_33_STABLE >> moodle_finishup

# Moosh
sudo -u www-data git clone git://github.com/tmuras/moosh.git
cd moosh
php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');"
php -r "if (hash_file('SHA384', 'composer-setup.php') === '669656bab3166a7aff8a7506b8cb2d1c292f042046c5a994c43155c0be6190fa0355160742ab2e1c88d40d5be660b410') { echo 'Installer verified'; } else { echo 'Installer corrupt'; unlink('composer-setup.php'); } echo PHP_EOL;"
php composer-setup.php
php -r "unlink('composer-setup.php');"
php composer.phar install
ln -sf $PWD/moosh.php /usr/local/bin/moosh

cd $STARTDIR


ln -sf /srv/mcweb/McWeb/scripts/uwsgi_mcweb /etc/init.d/uwsgi_mcweb
update-rc.d uwsgi_mcweb defaults

# Install and configure LDAP
apt-get install slapd ldap-utils
echo
echo
echo PLEASE REMEMBER / STORE your LDAP admin password as well as
echo 
LDAPDOMAIN=`slapcat |grep dn:\ dc|cut -f2- -d\ `
echo 1\) Your LDAP "domainname": $LDAPDOMAIN
echo
LDAPADMIN=`slapcat |grep admin | grep dn:\ cn=admin | cut -f2- -d\ `
echo 2\) Your LDAP admin "username": $LDAPADMIN
echo
sleep 10
export LDAPDOMAIN
echo

# Last setup of uwsgi etc
echo Resuming setup...
sed -i.bak "s/dc=risoe,dc=dk/${LDAPDOMAIN}/g" /srv/mcweb/McWeb/mcsimrunner/mcweb/settings.py

echo 

cd /srv/mcweb/
echo "***************************************************************************"
echo Dropping you to www-data shell where you should perform the bewlow commands
echo "***************************************************************************"
echo
echo
echo source /srv/mcweb/mcvenv/bin/activate
echo sh mcvenv_finishup
echo sh McWeb_finishup
sudo -u www-data -s