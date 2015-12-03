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

mcrun

mcdisplay

mcplot-gnuplot-py
