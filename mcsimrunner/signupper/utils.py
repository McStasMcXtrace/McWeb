'''
utils for signupper
'''
from mcweb import settings
import subprocess
from models import Signup
from datetime import datetime
import os
import re
from django.utils import timezone
from ldaputils import ldaputils
from moodleutils import moodleutils as mu

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
        signup.notified = timezone.now
    except Exception as e:
        signup.fail_str = signup.fail_str + ', notifyuser: %s' % str(e)
    signup.save()

def notifyuser(fullname, username, email, password):
    ''' send an account creation notification email to user '''
    body = '''
Dear %s

You have been added to the http://e-neutrons.org e-Learning system.

username: %s
password: %s

To change your password, please visit http://www.e-neutrons.org/ssp

Best,

The e-neutrons.org admin team
    ''' % (fullname, username, password)
    
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
            ldaputils.addsignup(signup)
        
        # try add to moodle
        if not s.is_in_moodle:
            mu.add_enroll_user(s.firstname, s.lastname, s.username, s.email, s.courses)
            s.is_in_moodle = True
            s.save()
        
        # try notify user if it is clearly added
        if s.is_in_ldap and s.is_in_moodle and not s.notified:
            notifyuser(s.firstname + ' ' + s.lastname, s.username, s.email, s.password)
            s.notified = timezone.now()
            s.save()
        
    except Exception as e:
        s.fail_str = '%s\n%s' % (s.fail_str, e.__str__())
        print s.fail_str
        s.save()

