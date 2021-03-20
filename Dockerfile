FROM ubuntu:18.04

ENV DEBIAN_FRONTEND="noninteractive" TZ="Etc/UTC"
RUN sed -i.bak s/main/main\ contrib\ non-free/g /etc/apt/sources.list

RUN apt-get update \
    && apt-get install -y net-tools iproute2 sudo openssh-server xbase-clients zip unzip wget \
    && rm -rf /var/lib/apt/lists/*

RUN cd /etc/apt/sources.list.d/ && wget http://packages.mccode.org/debian/mccode.list

RUN apt-get update \
    && apt-get install -y mcstas-suite-perl mcstas-suite-python mcxtrace-suite-perl \
        mcxtrace-suite-python openmpi-bin libopenmpi-dev mcstas-tools-python-mcplot-svg-2.7 \
    && rm -rf /var/lib/apt/lists/*

# Ensure we use mcdoc.pl rather than python version
RUN ln -sf /usr/share/mcstas/2.6/bin/mcdoc.pl /usr/bin/mcdoc

# Ensure mcplot.pl uses "proper" PGPLOT rather than GIZA
RUN cd /usr/lib/x86_64-linux-gnu \
    && ln -sf ../libpgplot.so libpgplot.so.0 \
    && ln -sf ../libcpgplot.so libcpgplot.so.0

# Packages for bootstrapping an McWeb instance
RUN apt-get update \
    && apt-get -y install git libsasl2-dev python-dev libldap2-dev libssl-dev \
        python-virtualenv makepasswd nginx php-fpm php-mysql php-xml php-curl \
        php-zip php-gd php-mbstring php-xmlrpc php-soap php-intl php-ldap \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir /srv/mcweb && chown -R www-data:www-data /srv/mcweb /var/www/

COPY scripts/uwsgi_mcweb  /etc/init.d/uwsgi_mcweb
COPY scripts/nginx-default /etc/nginx/sites-enabled/default

RUN update-rc.d uwsgi_mcweb defaults

RUN echo "\n# Allow www-data to restart uwsgi_mcweb service\nwww-data ALL = NOPASSWD: /etc/init.d/uwsgi_mcweb" \
    >> /etc/sudoers

WORKDIR /srv/mcweb

RUN apt-get update \
    && apt-get -y install git python3-dev python3-venv \
    && rm -rf /var/lib/apt/lists/*

RUN python3 -m venv mcvenv \
    && mcvenv/bin/pip install Django==1.8.2 django-auth-ldap==1.2.7 simplejson python-ldap uwsgi

# Copy files from this repo for the server
COPY --chown=www-data:www-data mcsimrunner /srv/mcweb/McWeb/mcsimrunner
COPY --chown=www-data:www-data landingpage/landingpage.in.html /srv/mcweb/landing/landingpage.in.html
RUN sed "s/@HOSTNAME@/$(hostname)/g" landing/landingpage.in.html > landing/index.html

RUN mkdir McWeb/mcsimrunner/sim/intro-ns \
    && cp /usr/share/mcstas/2.7/examples/templateSANS.instr /srv/mcweb/McWeb/mcsimrunner/sim/intro-ns/SANSsimple.instr \
    && chown --recursive www-data:www-data McWeb/mcsimrunner/sim/intro-ns

COPY --chown=www-data:www-data scripts/django-setup-test.sh django-setup-test.sh
