'''
Wrapper functions for calling mooshe on the terminal.

moosh example commands:
    sudo -u www-data moosh user-create --auth=ldap --firstname=John --lastname=Doe --city=Lyngby --country=DK --email=john@doe.com --password=NONE johndoe
    sudo -u www-data moosh course-enrol -s lib johndoe
    sudo -u www-data moosh course-enrol -s musr johndoe
    sudo -u www-data moosh course-enrol -r editingteacher -s intro-ns munivisit
    sudo -u www-data moosh user-list "username='munivisit'"
'''
import subprocess
from mcweb.settings import MOODLE_DIR

def add_enroll_user(firstname, lastname, username, email, courses_lst):
    ''' add user to moodle and _enroll in courses in courses_lst '''
    _adduser(firstname, lastname, username, email)
    for course in courses_lst:
        _enroll(username, course)

def _adduser(firstname, lastname, username, email):
    ''' add non-moodle-existing user to moodle '''
    cmd_adduser = 'moosh user-create --auth=ldap --firstname=%s --lastname=%s --city=Lyngby --country=DK --email=%s --password=NONE %s' % (firstname, lastname, email, username)
    subprocess.call(cmd_adduser, cwd=MOODLE_DIR, shell=True)

def _enroll(username, course):
    ''' _enroll moodle-existing user in course '''
    cmd_addtocourse = 'moosh course-enrol -s %s %s' % (course, username)
    subprocess.call(cmd_addtocourse, cwd=MOODLE_DIR, shell=True)
