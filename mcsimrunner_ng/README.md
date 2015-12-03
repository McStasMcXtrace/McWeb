# simrunner:

To install,  use: 

        $ python manage.py migrate
        $ python manage.py createsuperuser
        $ python manage.py collect_instr

The collect_instr command populates the db with groups and instruments based on the sim folder. To re-create based on another set of folders with instruments, first use the admin interface to delete all InstrGroup and Instrument objects. This will not require any SimRun objects to be deleted, but they may no longer refer to existing instrument objects or files.

To run, use in separate shells:

        $ python manage.py runworker
        $ python manage.py runserver

# admin site (and deployment)

In order to get css, js and so on (static files and content) working on the admin site, and in deployment, run:

        $ python manage.py collectstatic

Say "yes" to overwrite existing files. Static content should either be hosted in apps, and will be copied to a subfolder by that name.

# requirements: 

mcrun

mcdisplay

mcplot-gnuplot-py

etc.
