Config files for "self service password", see http://ltb-project.org/wiki/documentation/self-service-password/0.9/start.

To install / configure:
Download Debian package from http://ltb-project.org/wiki/documentation/self-service-password/0.9/start and install it:

$ wget http://tools.ltb-project.org/attachments/download/801/self-service-password_0.9-1_all.deb
$ dpkg -i self-service-password_0.9-1_all.deb
$ aptitude install php5 php5-ldap php5-mcrypt

* Package php files will be installed to /usr/share/self-service-password/
* Put/edit config.inc.php from ./conf in /usr/share/self-service-password/conf/config.inc.php
* Put style.css and e-neutrons-logo-blue-small.png from ./style in /usr/share/self-service-password/style

* Enable as ordinary php application in Nginx, e.g. by putting a link to the folder below the webroot (e.g. in /srv/mcweb), and creating a location with an alias for that link.
