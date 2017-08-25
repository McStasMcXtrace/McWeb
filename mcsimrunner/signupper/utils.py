'''
utils for signupper
'''
from mcweb import settings
from subprocess import Popen, PIPE, call
from models import Signup
from datetime import datetime
import os
import csv
import re
from django.utils import timezone
from ldaputils import ldaputils
from mcweb.settings import MCWEB_LDAP_DN
from moodleutils import moodleutils as mu

def get_random_passwd():
    ''' get a random password from the shell using makepasswd '''
    try:
        process = Popen('makepasswd', stdout=PIPE, stderr=PIPE, shell=True)
        (stdoutdata, stderrdata) = process.communicate()
        return stdoutdata.strip()
    except:
        print 'get_random_passwd() --> Popen("makepasswd") output:\n' + stderrdata
        raise

def cols_to_line(cols, delimiter = ','):
    ''' makes a list of strings into a single ;-separated string with a trailing linebreak '''
    line = ""
    for c in cols:
        line = "%s%s%s" % (line, c, delimiter)
    return line.rstrip("%s" % delimiter) + "\n"

def get_colheaders_moodle():
    ''' Returns a list of the standard signup column headers. The field "description" is used for deciding if an email has been sent '''
    header_cols = ["firstname", "lastname", "username", "email", "password", "description", "auth"]
    num_non_course = 7
    for i in range(len(settings.COURSES)):
        header_cols.append('course%s' % i)
    for i in range(len(settings.COURSES_MANDATORY)):
        header_cols.append('course%s' % i)
    return (header_cols, num_non_course)

def get_colheaders():
    ''' returns a list of the standard signup column headers used for display on the web-based add users interface '''
    header_cols = ['date', 'firstname', 'lastname', 'email', 'username', 'passwd']
    num_non_course = 5
    for c in settings.COURSES:
        header_cols.append(c)
    for c in settings.COURSES_MANDATORY:
        header_cols.append(c)
    return (header_cols, num_non_course)

def get_signups():
    ''' return relevant signup objects '''
    return Signup.objects.filter(is_new=True)

def get_signups_added():
    ''' return relevant signup objects '''
    return Signup.objects.filter(is_added=True)

def get_signups_limbo():
    ''' return relevant signup objects '''
    return Signup.objects.filter(is_limbo=True)

def create_signup(firstname, lastname, email, username, courses_lst, is_self_signup=False):
    ''' most simple way of creating a new signup instance '''
    signup = Signup(firstname=firstname, lastname=lastname, email=email, username=username, password=get_random_passwd(), is_self_signup=is_self_signup)
    signup.courses = courses_lst
    signup.save()
    return signup

def gettodaystr():
    ''' this string is part of the filename of csv files '''
    return datetime.now().strftime("%Y%m%d")

def notifyuser(fullname, username, email, password):
    ''' send an account creattion notification email to user '''
    body = '''
Dear %s

You have been added to the http://e-neutrons.org e-Learning system.

username: %s
password: %s

To change your password, please visit http://www.e-neutrons.org/ssp/

Best,

The e-neutrons.org admin team
    ''' % (fullname, username, password)
    
    try:
        f = open('_body', 'w') 
        f.write(body)
        f.close()
        cmd = 'mailx -s "welcome to mcweb" %s < _body' % email
        retcode = call(cmd, shell=True)
        print(cmd)
        if retcode != 0:
            raise Exception('retcode: %s' % retcode) 
    finally:
        os.remove('_body')

def notify_contactentry(replyto, text):
    ''' send a new contact entry notification to mcweb admin '''
    body = text    
    try:
        f = open('_contactbody', 'w') 
        f.write(body.encode('utf8'))
        f.close()
        cmd = 'mail.mailutils -s "mcweb: new contact entry by %s" -r "%s" %s < _contactbody' % (replyto, replyto, settings.MCWEB_ADMIN_EMAIL)
        retcode = call(cmd, shell=True)
        print(cmd)
        if retcode != 0:
            raise Exception('retcode: %s' % retcode)
    
    except Exception as e:
        raise Exception('notify_contactentry: %s' % e.message)
    
    finally:
        os.remove('_contactbody')

def pull_signups_todb(file, courses_all=None, courses_mandatory=None, courses_only=None):
    ''' creates unsaved signup instances from a csv file '''
    r = csv.reader(file, delimiter=',')
    firstname_idx = 0
    lastname_idx = 1
    email_idx = 2
    username_idx = 3
        
    first_row = True
    for row in r:
        # header line
        print(row)
        if first_row:
            firstname_idx = row.index('firstname')
            lastname_idx = row.index('lastname')
            email_idx = row.index('email')
            username_idx = row.index('username')
            first_row = False
            continue
        
        courses = []
        if not courses_only:
            # match courses in row
            for known in courses_all:
                if known in row:
                    courses.append(known)
            if courses_mandatory:
                courses = courses + courses_mandatory
        else:
            # hard-assign to courses_only
            courses = courses_only
        
        signup = create_signup(row[firstname_idx], row[lastname_idx], row[email_idx], row[username_idx], courses)


def update_signups(signups, form):
    ''' updates signup objects from signups, according to form, and saves '''
    # get course headers 
    headers, noncourses = get_colheaders()
    courseheaders = headers[noncourses:]
    
    # get and save new configuration for each signup
    for s in signups:
        # get checked courses
        courses = []
        for course in courseheaders:
            # NOTE: non-checked checkboxes do not become included in the form submission
            cbx = form.get('%s_%s' % (str(s.id), course))
            if cbx:
                courses.append(course)
        s.courses = courses
        s.save()
        
        # get name, email and username fields
        s.firstname = form.get('%s_%s' % (str(s.id), 'firstname'))
        s.lastname = form.get('%s_%s' % (str(s.id), 'lastname'))
        s.email = form.get('%s_%s' % (str(s.id), 'email'))
        s.username = form.get('%s_%s' % (str(s.id), 'username'))
        s.save()


def assign_courses(signups, courses):
    ''' adds strings in courses (a list) to each signup in signups (a list) '''
    for s in signups:
        s.courses = s.courses + courses

def adduser(signup, ldap_password, accept_ldap_exists=False):
    ''' 
    Adds signup to ldap and moodle, and enrolls to the selected moodle courses.
    
    accept_ldap_exists : if True, accept ldap error "Already exists", but is_in_ldap property is untouched.
    '''
    s = signup
    try:
        # try add to ldap
        if not s.is_in_ldap:
            try:
                ldaputils.adduser(MCWEB_LDAP_DN, ldap_password, s.firstname, s.lastname, s.username, s.email, s.password)
                s.is_in_ldap = True
                s.save()
            except Exception as e:
                if not accept_ldap_exists:
                    raise e
                m = re.search(r'Already exists', e.__str__())
                if not m:
                    raise e
        
        # try add to moodle
        mu.add_enroll_user(s.firstname, s.lastname, s.username, s.email, s.courses)
        
        # try notify user
        if not s.notified:
            notifyuser(s.firstname + ' ' + s.lastname, s.username, s.email, s.password)
            s.notified = timezone.now()
            s.save()
        
    except Exception as e:
        s.fail_str = '%s\n%s' % (s.fail_str, e.__str__())
        print s.fail_str
        s.save()
