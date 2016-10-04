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
import re
import os
from mcweb.settings import MOODLE_DIR

TEMPLATES_DIR = "/srv/mcweb/moodle-course-templates"
DEFAULT_CATEGORY_ID = '1'

def add_enroll_user(firstname, lastname, username, email, courses_sn_lst):
    '''
    add user to moodle and _enroll in courses in courses_lst
    '''
    _adduser(firstname, lastname, username, email)
    for course in courses_sn_lst:
        _enroll(username, course)

def create_template(shortname, templatename):
    '''
    creates a template
    '''
    # get course id and category id for the course matching shortname
    courses = _course_list()
    course = None
    for c in courses:
        if shortname == c[2]:
            course = c[2]
    course_id = course[0]
    category_id = course[1]
    
    # check that template name is unique
    tmplts = _get_templates()
    if templatename in tmplts:
        raise Exception("create_template: Template names must be unique.")
    
    # create the template
    _course_backup(backupname=templatename, course_id=course_id)
    
def get_courses():
    '''
    returns a list of course shortnames for high-level use
    '''
    lst = []
    for c in _course_list():
        lst.append(c[2])
    return lst

def create_course_from_template(templatename, shortname, fullname):
    '''
    High-level function to create a new course from a template, with the right names.
    
    Returns the id of the new course, which can be used to add users.
    '''
    # create empty course with the right shortname/fullname
    _course_create(shortname=shortname, fullname=fullname, category_id='1')
    
    # get the course id of the newly created course
    lst = _course_list()
    id = ''
    for c in lst:
        if shortname == c[2]:
            id = c[0]
    
    # restore to the newly created course 
    _course_restore_e(backupname=templatename, course_id=id)
    
    return id

def _adduser(firstname, lastname, username, email):
    cmd_adduser = 'moosh user-create --auth=ldap --firstname=%s --lastname=%s --city=Lyngby --country=DK --email=%s --password=NONE %s' % (firstname, lastname, email, username)
    subprocess.call(cmd_adduser,
                    cwd=MOODLE_DIR, 
                    shell=True)

def _enroll(username, course_sn):
    cmd_addtocourse = 'moosh course-enrol -s %s %s' % (course_sn, username)
    subprocess.call(cmd_addtocourse, 
                    cwd=MOODLE_DIR, 
                    shell=True)

def _course_list():
    '''
    returns: ["id","category","shortname","fullname","visible"]
    '''
    proc = subprocess.Popen('moosh course-list', 
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR, 
                            shell=True)
    com = proc.communicate()
    
    unit = '[/\-\w\s]+'
    spat = r'"(%s)","(%s)","(%s)","(%s)","(%s)"' % (unit, unit, unit, unit, unit)
    
    v_lst = []
    text = com[0]
    m_lst = text.splitlines()
    for l in m_lst:
        try:
            a = re.search(spat, l)
            v_lst.append([a.group(1),a.group(3),a.group(4)])
        except:
            break
    return v_lst

def _get_templates():
    for (a, b, files) in os.walk(TEMPLATES_DIR):
        return files

def _course_backup(backupname, course_id):
    proc = subprocess.Popen('moosh course-backup --template -f %s.mbz %s' % (os.path.join(TEMPLATES_DIR, backupname), str(course_id)),
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR,
                            shell=True)
    text = proc.communicate()
    print text

def _course_restore_e(backupname, course_id):
    proc = subprocess.Popen('moosh course-restore -e %s.mbz %s' % (os.path.join(TEMPLATES_DIR, backupname), str(course_id)), 
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR, 
                            shell=True)
    text = proc.communicate()
    print text

def _course_create(shortname, fullname, category_id):
    proc = subprocess.Popen('moosh course-create %s --fullname=%s --category=%s --visible=y' % (shortname, fullname, str(category_id)),
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR,
                            shell=True)
    text = proc.communicate()
    print text

