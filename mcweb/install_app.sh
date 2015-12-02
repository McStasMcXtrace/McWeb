#!/bin/bash

# get and install app, set permissions
git clone https://github.com/McStasMcXtrace/McWeb
cd McWeb
python manage.py syncdb
python manage.py generate_docs
./bin/update-simulations.sh
python manage.py populate_db
cd ..
sudo chown -R root:www-data McWeb
sudo chmod -R g+w McWeb

