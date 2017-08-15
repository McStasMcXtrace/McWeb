#!/bin/sh

STARTDIR=`pwd`
# Basic infrastructure
apt-get -y install net-tools sudo openssh-server xbase-clients

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
apt-get -y install mcstas-suite-perl mcstas-suite-python openmpi-bin libopenmpi-dev

echo "deb http://repo.mysql.com/apt/debian/ stretch mysql-5.7" > /etc/apt/sources.list.d/mysql.list
echo "deb-src http://repo.mysql.com/apt/debian/ stretch mysql-5.7" >> /etc/apt/sources.list.d/mysql.list
wget -O /tmp/RPM-GPG-KEY-mysql https://repo.mysql.com/RPM-GPG-KEY-mysql
apt-key add /tmp/RPM-GPG-KEY-mysql
apt-get update
apt-get -y install mysql-server mysql-client

# Remove stop apache2 from being default webserver
update-rc.d apache2 remove
service apache2 stop

# Packages for bootstrapping an McWeb instance
apt-get -y install git libsasl2-dev python-dev libldap2-dev libssl-dev python-virtualenv makepasswd nginx php-fpm php-mysql php-xml php-curl php-zip php-gd php-mbstring php-xmlrpc php-soap php-intl

rm -rf /srv/mcweb
mkdir /srv/mcweb
mkdir -p /srv/moodledata
sudo chown -R www-data:www-data /srv/mcweb /var/www/ /srv/moodledata

# Bootstrap McWeb via sudo / git
cd /srv/mcweb
sudo -u www-data -H virtualenv mcvenv
sudo -u www-data cp mcvenv/bin/activate > mcvenv_finishup
echo pip install -I Django==1.8.2 >> mcvenv_finishup
echo pip install simplejson django_auth_ldap uwsgi python-ldap >> mcvenv_finishup
sudo -u www-data bash mcvenv_finishup

sudo -u www-data git clone https://github.com/McStasMcXtrace/McWeb
cd McWeb/mcsimrunner/

sudo -u www-data cp mcvenv/bin/activate > McWeb_finishup
echo cd McWeb/mcsimrunner/ >> McWeb_finishup
echo python manage.py migrate >> McWeb_finishup
echo python manage.py collect_instr >>  McWeb_finishup
echo echo Please assist Django in creating a local superuser account: >>  McWeb_finishup
echo echo python manage.py createsuperuser >>  McWeb_finishup
echo python manage.py createsuperuser >>  McWeb_finishup
echo echo Please assist Django in creating the final LDAP: >>  McWeb_finishup
echo echo \\\> python manage.py ldap_initdb USE_YOUR_LDAP_PASSWD_HERE >>  McWeb_finishup
echo echo >>  McWeb_finishup
echo echo Afterwards, please sudo /etc/init.d/uwsgi_mcweb start >>  McWeb_finishup

cat /srv/mcweb/McWeb/scripts/nginx-default > /etc/nginx/sites-enabled/default
service nginx restart

# Moodle
cd $STARTDIR
sudo -u www-data git clone https://github.com/moodle/moodle.git
cd moodle
sudo -u www-data git checkout MOODLE_33_STABLE

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
apt-get -y install slapd ldap-utils
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

cd /srv/mcweb/
echo "***************************************************************************"
echo Dropping you to www-data shell where you should perform the bewlow commands
echo "***************************************************************************"
echo
echo
echo source /srv/mcweb/mcvenv/bin/activate
echo sh McWeb_finishup
sudo -u www-data bash McWeb_finishup

#echo Please secure your mysql installation below:
#mysql_secure_installation