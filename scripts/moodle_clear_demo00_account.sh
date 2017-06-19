cd /srv/mcweb/moodle
moosh user-delete demo00
moosh user-create --auth=ldap --firstname=Demo --lastname=Account --city=Lyngby --country=DK --email=demo00@e-neutrons.org --password=NONE demo00
moosh course-enrol -r student -s intro-ns demo00
