'''
utils for signupper
'''
from mcweb import settings
import subprocess
from models import Signup
from datetime import datetime
import os
from django.utils import timezone
from ldaputils import ldaputils
from moodleutils import moodleutils as mu
from mcweb.settings import MCWEB_NOTIFY_EMAIL_URL, MCWEB_SSP_URL

def get_random_passwd():
    ''' get a random password from the shell using makepasswd '''
    cmd = 'mktemp -u XXXXXXXX'
    proc = subprocess.Popen(cmd, 
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            shell=True)
    com = proc.communicate()
    std_err = com[1]
    print('running: %s' % cmd)
    print('std-out: %s' % com[0])
    if std_err != '':
        print('std-err: %s' % std_err)
        raise Exception('makepasswd says: %s' % std_err)
    
    return com[0].strip()

def cols_to_line(cols, delimiter = ','):
    ''' makes a list of strings into a single ;-separated string with a trailing linebreak '''
    line = ""
    for c in cols:
        line = "%s%s%s" % (line, c, delimiter)
    return line.rstrip("%s" % delimiter) + "\n"

def get_new_signups():
    return [s for s in Signup.objects.all() if s.state() == 1 or s.state() == 2]

def get_signups_inldap():
    return Signup.objects.filter(is_in_ldap=True)

def get_signups_all():
    return Signup.objects.all()

def get_signups_self():
    return Signup.objects.filter(is_self_signup=True)

def gettodaystr():
    ''' this string is part of the filename of csv files '''
    return datetime.now().strftime("%Y%m%d")

def notify_signup(signup):
    ''' signup-specific proxy to the notifyuser function, also saves '''
    try:
        notifyuser(signup.firstname + ' ' + signup.lastname, signup.username, signup.email, signup.password)
        signup.notified = timezone.now()
    except Exception as e:
        signup.fail_str = signup.fail_str + ', notifyuser: %s' % str(e)
    signup.save()

def notifyuser(fullname, username, email, password, courses):
    ''' send an account creation notification email to user '''
    
    courses_text = ''
    if len(courses) > 0:
        allcourses = mu.course_list()
        
        # id, shortname, fullname
        courses_info = [c for c in allcourses if c[1] in courses]
        
        courses_text = '\nYou have been enroled in the following courses:\n'
        for c in courses_info:
            courses_text = courses_text + '\n%s: %s/moodle/course/view.php?id=%s' % (c[2], MCWEB_NOTIFY_EMAIL_URL, c[0])
        courses_text = courses_text + '\n'
    
    body = '''
Dear %s

You have been added to the %s e-Learning system.

username: %s
password: %s

To change your password, please visit %s
%s
Best,

The e-neutrons.org admin team
    ''' % (fullname, MCWEB_NOTIFY_EMAIL_URL, username, password, MCWEB_SSP_URL, courses_text)
    
    try:
        f = open('_body', 'w') 
        f.write(body)
        f.close()
        
        cmd = 'mailx -s "welcome to mcweb" %s < _body' % email
        retcode = subprocess.call(cmd, shell=True)
        print(cmd)
        
        if retcode != 0:
            raise Exception('notifyuser mailx retcode: %s' % retcode) 
    finally:
        os.remove('_body')

def notify_contactentry(replyto, text):
    ''' send a new contact entry notification to mcweb admin '''
    body = text    
    try:
        f = open('_contactbody', 'w') 
        f.write(body.encode('utf8'))
        f.close()
        
        cmd = 'mailx -s "mcweb: new contact entry by %s" -r "%s" %s < _contactbody' % (replyto, replyto, settings.MCWEB_ADMIN_EMAIL)
        retcode = subprocess.call(cmd, shell=True)
        print(cmd)
        
        if retcode != 0:
            raise Exception('notify_contactentry mail.mailutils retcode: %s' % retcode)
    
    except Exception as e:
        raise Exception('notify_contactentry: %s' % e.message)
    
    finally:
        os.remove('_contactbody')

def pull_csv_signups_todb(f):
    ''' creates unsaved signup instances from a csv file '''
    # read data
    text = ''.join(f.chunks())
    lines = text.splitlines()[1:]
    
    # generate signups
    signups = []
    for l in lines:
        words = l.split(',')
        signup = Signup(firstname=words[0], lastname=words[1], email=words[2], username=words[3], password=get_random_passwd(), courses=[])
        signup.save()
        signups.append(signup)
    
    return signups

def assign_courses(signups, courses):
    ''' adds strings in courses (a list) to each signup in signups (a list) '''
    for s in signups:
        s.courses = s.courses + courses

def adduser(signup):
    ''' Adds signup to ldap and moodle, and enrolls to the selected moodle courses. Updates signup state. '''
    s = signup
    try:
        # try add to ldap
        if not s.is_in_ldap:
            ldaputils.addsignup(s)
        
        # try add to moodle
        if not s.is_in_moodle:
            mu.add_enrol_user(s.firstname, s.lastname, s.username, s.email, s.courses)
            s.is_in_moodle = True
            s.save()
        
        # try notify user
        if s.is_in_ldap and s.is_in_moodle and not s.notified:
            notifyuser(s.firstname + ' ' + s.lastname, s.username, s.email, s.password, courses=s.courses)
            s.notified = timezone.now()
            s.save()
        
    except Exception as e:
        s.fail_str = '%s\n%s' % (s.fail_str, e.__str__())
        print s.fail_str
        s.save()

def enroluser(signup, course_sn, teacher=False):
    ''' safe proxy function to moodleutils.enroluser which appends to the signup fail-string '''
    try:
        mu.enrol_user(signup.username, course_sn, teacher)
    except Exception as e:
        signup.fail_str = '%s\n%s' % (signup.fail_str, e.__str__())
        signup.save()

def get_courses():
    ''' safe proxy call to moodleutils.course_list '''
    return ['dummy']
    try:
        lst = []
        for c in mu.course_list():
            lst.append(c[1])
        return lst
    except:
        return []

def get_templates():
    ''' walks files on disk in the directory which is supposed to hold course templates '''
    for (a, b, files) in os.walk(mu.TEMPLATES_DIR):
        return files

def create_template(shortname, templatename):
    ''' safe proxy to moodleutils.course_backup '''
    course_id = get_course_id(shortname)
    if not course_id:
        return 'no course of that shortname'
    
    # check that template name is unique
    tmplts = get_templates()
    if templatename in tmplts:
        return "utils.create_template: Template names must be unique."
    
    # create the template
    mu.course_backup(backupname=templatename, course_id=course_id)
    
    return 'success.'

def get_course_id(shortname):
    ''' returns the course id of the given course identified by shortname, or None if this does not exist '''
    course_id = None
    courses = mu.course_list()
    for c in courses:
        if shortname == c[1] and not course_id:
            course_id = c[0]
        elif shortname == c[1]:
            raise Exception("something is terribly wrong in moodle")
    
    return course_id

def create_course_from_template(templatename, shortname, fullname):
    ''' high-level function to create a new course from a template, returning the id of the new course. Does not call restore for already-existing courses '''
    course_id = get_course_id(shortname)
    if not course_id:
        mu.course_create(shortname=shortname, fullname=fullname)
        course_id = get_course_id(shortname)
    else:
        return 'course id already exists'
    
    if course_id:
        return mu.course_restore_e(backupname=templatename, course_id=course_id)
    else:
        return 'could not get new course id'

def get_signup(username):
    ''' a simple db search filter '''
    qs = Signup.objects.filter(username=username)
    if len(qs) == 1:
        return qs[0]

def purgeuser(signup):
    ''' removes the user from ldap and moodle, marking signup instance as deleted '''
    ldap_e = None
    if signup.is_in_ldap:
        try:
            ldaputils.rmsignup(signup.username)
            signup.is_in_ldap = False
            signup.save()
        except Exception as e:
            ldap_e = e
            return 'Error during ldaputils.rmsignup: %s' % e.__str__()
    
    moodle_e = None
    if signup.is_in_moodle:
        try:
            mu.rmsignup(signup)
            signup.is_in_moodle = False
            signup.save()
        except Exception as e:
            moodle_e = e
            return 'Error during moodleutils.rmsignup: %s' % e.__str__()
    
    if ldap_e and moodle_e:
        raise Exception('%s\n%s' % (ldap_e.__str__(), moodle_e.__str__()))
    elif ldap_e:
        raise ldap_e
    elif moodle_e:
        raise moodle_e
    
    signup.deleted = timezone.now()
    signup.save()
    return 'The given user was purged from moodle and ldap.'

def purgeusers(signups):
    ''' list-based proxy for purgeuser, which also handles errors smoothly '''
    errors = []
    for signup in signups: 
        try:
            purgeuser(signup)
        except Exception as e:
            errors.append(e.__str__())
    if len(errors) > 0:
        return 'Errors were encountered for some signups: \n'.join(errors)
    else:
        return 'purge successful'

