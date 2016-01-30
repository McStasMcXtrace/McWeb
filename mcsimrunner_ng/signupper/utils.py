'''
utils for signupper
'''
from mcweb import settings
from subprocess import Popen, PIPE
from models import Signup

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
    header_cols = ['date', 'firstname', 'lastname', 'email', 'username']
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

def create_signup(firstname, lastname, email, username, courses_lst):
    ''' most simple way of creating a new signup instance '''
    
    signup = Signup(firstname=firstname, lastname=lastname, email=email, username=username, password=get_random_passwd())
    signup.courses = courses_lst
    signup.save()
    return

def moodle_adduser():
    pass

def notifyuser():
    pass
