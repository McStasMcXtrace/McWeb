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
import tempfile
from mcweb.settings import MOODLE_DIR

TEMPLATES_DIR = "/srv/mcweb/moodle-course-templates"
DEFAULT_CATEGORY_ID = '1'
MOODLE_RESTORE_JOBS_DIR = '/srv/mcweb/moodle-restore-jobs'

def add_enroll_user(firstname, lastname, username, email, courses_sn_lst):
    ''' Add user to moodle and enroll_user in courses in courses_lst '''
    adduser(firstname, lastname, username, email)
    for course in courses_sn_lst:
        enroll_user(username, course)

def create_template(shortname, templatename):
    ''' Creates a template '''
    # get course and category id
    courses = _course_list()
    course = None
    for c in courses:
        if shortname == c[1]:
            course = c   
    course_id = course[0]

    # check that template name is unique
    tmplts = get_templates()
    if templatename in tmplts:
        raise Exception("create_template: Template names must be unique.")

    # create the template
    _course_backup(backupname=templatename, course_id=course_id)
    
def get_courses():
    ''' Returns a list of course names for high-level use. '''
    lst = []
    for c in _course_list():
        lst.append(c[1])
    return lst

def create_course_from_template(templatename, shortname, fullname):
    ''' High-level function to create a new course from a template, returning the id of the new course. '''
    _course_create(shortname=shortname, fullname=fullname, category_id=DEFAULT_CATEGORY_ID)
    
    # get course id of the newly created course
    lst = _course_list()
    id = ''
    for c in lst:
        if shortname == c[1]:
            id = c[0]
    
    # TODO: check that the course exists, or exit with an error
    
    message = _course_restore_e(backupname=templatename, course_id=id)
    return templatename, id, message

def adduser(firstname, lastname, username, email):
    '''  '''
    cmd = 'moosh user-create --auth=ldap --firstname=%s --lastname=%s --city=Lyngby --country=DK --email=%s --password=NONE %s' % (firstname, lastname, email, username)
    proc = subprocess.Popen(cmd, 
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR, 
                            shell=True)
    com = proc.communicate()
    print('running: %s' % cmd)
    print('std-out: %s' % com[0])
    if com[1] != '':
        print('std-err: %s' % com[1])

def enroll_user(username, course_sn, teacher=False):
    '''  '''
    if not teacher:
        cmd = 'moosh course-enrol -r student -s %s %s' % (course_sn, username)
    else:
        cmd = 'moosh course-enrol -r editingteacher -s %s %s' % (course_sn, username)
        
    proc = subprocess.Popen(cmd, 
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR, 
                            shell=True)
    com = proc.communicate()
    print('running: %s' % cmd)
    print('std-out: %s' % com[0])
    if com[1] != '':
        print('std-err: %s' % com[1])
        return com[1]

def _course_list():
    ''' returns: ["courseid","shortname","fullname"] '''
    cmd = 'moosh course-list'
    proc = subprocess.Popen(cmd, 
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR, 
                            shell=True)
    com = proc.communicate()
    print('running: %s' % cmd)
    print('std-out: %s' % com[0])
    if com[1] != '':
        print('std-err: %s' % com[1])
    
    unit = '[/\,\.\\\(\)\[\]\{\}\-\w\s]+'
    spat = r'"(%s)","(%s)","(%s)","(%s)","(%s)"' % (unit, unit, unit, unit, unit)
    
    v_lst = []
    text = com[0]
    m_lst = text.splitlines()
    for l in m_lst:
        try:
            a = re.search(spat, l)
            v_lst.append([a.group(1),a.group(3),a.group(4)])
        except:
            continue
    
    return v_lst[1:]

def get_templates():
    '''  '''
    for (a, b, files) in os.walk(TEMPLATES_DIR):
        return files

def _course_backup(backupname, course_id):
    '''  '''
    cmd = 'moosh course-backup --template -f %s.mbz %s' % (os.path.join(TEMPLATES_DIR, backupname), str(course_id))
    proc = subprocess.Popen(cmd,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR,
                            shell=True)
    com = proc.communicate()
    print('running: %s' % cmd)
    print('std-out: %s' % com[0])
    if com[1] != '':
        print('std-err: %s' % com[1])

def _course_restore_e(backupname, course_id):
    '''  '''
    tf = tempfile.NamedTemporaryFile()
    fname = os.path.basename(tf.name) + '.mrjob'
    
    f = open(os.path.join(MOODLE_RESTORE_JOBS_DIR, fname), 'w')
    cmd = 'moosh course-restore -e %s %s\n' % (os.path.join(TEMPLATES_DIR, backupname), str(course_id))
    f.write(cmd)
    print('writing to worker task: %s' % cmd)
    f.close()
    
    return "running"

def _course_create(shortname, fullname, category_id):
    ''' '''
    cmd = 'moosh course-create --fullname="%s" --category="%s" --visible="y" "%s"' % (fullname, str(category_id), shortname)
    proc = subprocess.Popen(cmd,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR,
                            shell=True)
    com = proc.communicate()
    print('running: %s' % cmd)
    print('std-out: %s' % com[0])
    if com[1] != '':
        print('std-err: %s' % com[1])

def synchronize(signups, dry=False, verbose=False):
    ''' attempt to sync is_in_moodle signup field to the moodle db '''
    cmd = 'moosh user-list "id > 0"'
    proc = subprocess.Popen(cmd,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR,
                            shell=True)
    com = proc.communicate()
    print('running: %s' % cmd)
    #print('std-out: %s' % com[0])
    if com[1] != '':
        print('std-err: %s' % com[1])
        raise Exception('Calling: moosh user-list "id > 0"')
    
    text = com[0]
    lines = text.splitlines()
    moodle_uids = []
    for l in lines: 
        try:
            pat = '([\w\@\-\.]+)\s\([0-9]+\)'
            uid = re.match(pat, l).group(1)
            moodle_uids.append(uid)
        except:
            raise Exception('Please improve moodleutils.syncronize regex "%s" with line "%s".' % (pat, l))
    
    subset = [s for s in signups if s.username in moodle_uids]
    disjoint = [s for s in signups if s not in subset]
    
    if not dry:
        for s in subset: s.is_in_moodle = True
        for s in disjoint: s.is_in_moodle = False
        for s in signups: s.save()
    else:
        print('')
        print("-- %d moodle uids --" % len(moodle_uids))
        print('')
        if verbose:
            for u in moodle_uids:
                print(u)
        
        print('')
        print('-- %d matches with local --' % len(subset))
        print('')
        if verbose:
            for u in subset:
                print(u)
