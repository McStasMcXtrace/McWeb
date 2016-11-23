# simrunner:

To install,  use: 

        $ python manage.py migrate
        $ python manage.py createsuperuser
        $ python manage.py collect_instr

The collect_instr command populates the db with groups and instruments based on the sim folder. To re-create based on another set of folders with instruments, first use the admin interface to delete all InstrGroup and Instrument objects. This will not require any SimRun objects to be deleted, but they may no longer refer to existing instrument objects or files.

NOTE: When updating database schemes (models.py files), run 'python manage.py makemigrations \<app\>' and commit the migration file, stored in \<app\>/migrations. This process is designed make 'migrate', as used above, work for all.

To run, use (in separate shells, to monitor stdout):

        $ python manage.py runworker
        $ python manage.py runserver

# admin site (and deployment)

In order to get css, js and so on (static files and content) working on the admin site, and in deployment, run:

        $ python manage.py collectstatic

Say "yes" to overwrite existing files. Static content should either be hosted in apps, in which case files will be copied to a subfolder by that name, or at another location in the static dir (for generic, project-wide static files).

# requirements: 

A recent version of McStas including the mcrun, mcdisplaym and mcplot (either oldschool PGPLOT mcplot or mcplot-gnuplot-py)

# Unconfigure use of whitedune for VRML display:
sudo sed -i s/whitedune/echo/g /usr/share/mcstas/VERSION/tools/Perl/perl/mccode_config.perl
(Or store in the local $HOME/.mcstas/mccode_config.perl of the webserver user)

# Incomplete: For WebGL based 3D rendering of the instrument install - probably requires Ubuntu 14.04!
InstantReality from http://www.instantreality.org/downloads/
(has these dependencies: freeglut3 libalut0 libavahi-compat-libdnssd1 libavcodec54 libavcodec-extra-54 libavformat54 libavutil52 libboost-system1.54.0 libboost-thread1.54.0)


