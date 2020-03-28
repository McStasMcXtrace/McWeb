'''
Wrapper functions for calling mooshe on the terminal.

/usr/local/bin/moosh example commands:
    sudo -u www-data /usr/local/bin/moosh user-create --auth=ldap --firstname=John --lastname=Doe --city=Lyngby --country=DK --email=john@doe.com --password=NONE johndoe
    sudo -u www-data /usr/local/bin/moosh course-enrol -s lib johndoe
    sudo -u www-data /usr/local/bin/moosh course-enrol -s musr johndoe
    sudo -u www-data /usr/local/bin/moosh course-enrol -r editingteacher -s intro-ns munivisit
    sudo -u www-data /usr/local/bin/moosh user-list "username='munivisit'"
'''
import subprocess
import re
import os
import tempfile
from mcweb.settings import MOODLE_DIR

TEMPLATES_DIR = "/srv/mcweb/moodle-course-templates"
DEFAULT_CATEGORY_ID = '1'
MOODLE_RESTORE_JOBS_DIR = '/srv/mcweb/moodle-restore-jobs'

def add_enrol_user(firstname, lastname, username, email, courses_sn_lst):
    ''' Add user to moodle and enrol_user in courses in courses_lst '''
    adduser(firstname, lastname, username, email)
    for course in courses_sn_lst:
        enrol_user(username, course)

def adduser(firstname, lastname, username, email):
    '''  '''
    cmd = '/usr/local/bin/moosh user-create --auth=ldap --firstname="%s" --lastname="%s" --city=Lyngby --country=DK --email=%s --password=NONE %s' % (firstname, lastname, email, username)
    proc = subprocess.Popen(cmd, 
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR, 
                            shell=True)
    com = proc.communicate()
    print('running: %s' % cmd)
    if com[0] != '':
        print('std-out: %s' % com[0])
    if com[1] != '':
        print('std-err: %s' % com[1])

def enrol_user(username, course_sn, teacher=False):
    '''  '''
    if not teacher:
        cmd = '/usr/local/bin/moosh course-enrol -r student -s %s %s' % (course_sn, username)
    else:
        cmd = '/usr/local/bin/moosh course-enrol -r editingteacher -s %s %s' % (course_sn, username)
        
    proc = subprocess.Popen(cmd, 
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR, 
                            shell=True)
    com = proc.communicate()
    print('running: %s' % cmd)
    if com[0] != '':
        print('std-out: %s' % com[0])
    if com[1] != '':
        print('std-err: %s' % com[1])
        raise Exception(com[1])

def course_list():
    ''' returns: ["courseid","shortname","fullname"] '''
    cmd = '/usr/local/bin/moosh course-list'
    proc = subprocess.Popen(cmd, 
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR, 
                            shell=True)
    com = proc.communicate()
    print('running: %s' % cmd)
    if com[0] != '':
        print('std-out: %s' % com[0])
    if com[1] != '':
        print('std-err: %s' % com[1])
        raise Exception(com[1])
    
    unit = '[/\,\.\\\(\)\[\]\{\}\-\w\s]+'
    spat = r'"(%s)","(%s)","(%s)","(%s)","(%s)"' % (unit, unit, unit, unit, unit)
    
    v_lst = []
    text = com[0]
    m_lst = text.splitlines()
    for l in m_lst:
        try:
            a = re.search(spat, l)
            v_lst.append([a.group(1), a.group(3), a.group(4)])
        except:
            continue
    
    return v_lst[1:]

def course_backup(backupname, course_id):
    '''  '''
    cmd = '/usr/local/bin/moosh course-backup --template -f %s.mbz %s' % (os.path.join(TEMPLATES_DIR, backupname), str(course_id))
    proc = subprocess.Popen(cmd,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR,
                            shell=True)
    com = proc.communicate()
    if com[0] != '':
        print('running: %s' % cmd)
    print('std-out: %s' % com[0])
    if com[1] != '':
        print('std-err: %s' % com[1])
        raise Exception(com[1])

def course_restore_e(backupname, course_id):
    ''' dumps a course-restore request to be picked up by a worker '''
    tf = tempfile.NamedTemporaryFile()
    fname = os.path.basename(tf.name) + '.mrjob'
    
    f = open(os.path.join(MOODLE_RESTORE_JOBS_DIR, fname), 'w')
    cmd = '/usr/local/bin/moosh course-restore -e %s %s\n' % (os.path.join(TEMPLATES_DIR, backupname), str(course_id))
    f.write(cmd)
    print('writing to worker task: %s' % cmd)
    f.close()
    
    cmd = '/srv/mcweb/McWeb/scripts/moodle_restoreme.sh &> /dev/null'
    proc = subprocess.Popen(cmd, shell=True)
    
    return "restoring contents in the background..."

def course_create(shortname, fullname, category_id=DEFAULT_CATEGORY_ID):
    ''' '''
    cmd = '/usr/local/bin/moosh course-create --fullname="%s" --category="%s" --visible="y" "%s"' % (fullname, str(category_id), shortname)
    proc = subprocess.Popen(cmd,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR,
                            shell=True)
    com = proc.communicate()
    print('running: %s' % cmd)
    if com[0] != '':
        print('std-out: %s' % com[0])
    if com[1] != '':
        print('std-err: %s' % com[1])
        # TODO: add an exception and exception-handling to the higher-up layer

def synchronize(signups, dry=False, verbose=False):
    ''' attempt to sync is_in_moodle signup field to the moodle db '''
    cmd = '/usr/local/bin/moosh user-list "id > 0"'
    proc = subprocess.Popen(cmd,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR,
                            shell=True)
    com = proc.communicate()
    print('running: %s' % cmd)
    if com[0] != '':
        print('std-out: %s' % com[0])
    if com[1] != '':
        print('std-err: %s' % com[1])
        raise Exception('Calling: /usr/local/bin/moosh user-list "id > 0"')
    
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

def rmsignup(signup):
    ''' removes the user associated with the given signup from moodle '''
    cmd = '/usr/local/bin/moosh user-delete %s' % signup.username
    proc = subprocess.Popen(cmd,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=MOODLE_DIR,
                            shell=True)
    com = proc.communicate()
    print('running: %s' % cmd)
    if com[0] != '':
        print('std-out: %s' % com[0])
    if com[1] != '':
        print('std-err: %s' % com[1])
        raise Exception(com[1])

