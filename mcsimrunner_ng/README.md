# simrunner:

To install,  use: 

        $ python manage.py migrate
        $ python manage.py createsuperuser
        $ python manage.py collect_instr

The collect_instr command populates the db with groups and instruments based on the sim folder. To re-create based on another set of folders with instruments, first use the admin interface to delete all InstrGroup and Instrument objects. This will not require any SimRun objects to be deleted, but they may no longer refer to existing instrument objects or files.

To run, use in separate shells:

        $ python manage.py runworker
        $ python manage.py runserver

# Requirements: 

mcrun

mcdisplay

mcplot-gnuplot-py

etc.
