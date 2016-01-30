'''
signupper views - NOTE: the get response is intended to show up in a _blank window, which is why it redirects to 
the thank you page.
'''
from django.shortcuts import render, redirect
from django.contrib.auth import authenticate, login
from django.contrib.auth.decorators import login_required
from django.http import HttpResponse
from django.utils import timezone

import ast
import os

import utils
from mcweb import settings
from mcweb.settings import MCWEB_LDAP_DN
from useradmin import ldap_chpassword
from models import Signup
from ldaputils import ldaputils


def signup(req):
    ''' displays the signup form '''
    return render(req, 'signup.html', {'courses': settings.COURSES})

def signup_get(req):
    ''' signup form GET parsing, and append the signup line to file new_signups.csv '''
    csv = 'new_signups.csv'
    
    # and the form
    form = req.GET
    
    # get static fields from the form
    firstname = form.get('firstname')
    lastname = form.get('lastname')
    username = form.get('username')
    email = form.get('email')
    password = utils.get_random_passwd()
    description = '0' # used for an email notification flag
    auth = 'ldap'
    cols = [firstname, lastname, username, email, password, description, auth]
    
    # get dynamic fields from the form
    for c in settings.COURSES:
        cols.append(form.get(c) or '')
    for c in settings.COURSES_MANDATORY:
        cols.append(c)
    
    # create the header line - an empty string if the file exists
    header_line = ""
    if not os.path.exists(csv):
        header_cols = ["firstname", "lastname", "username", "email", "password", "description", "auth"]
        i = 0
        for c in settings.COURSES:
            i += 1
            header_cols.append('course%s' % i)
        for c in settings.COURSES_MANDATORY:
            i += 1
            header_cols.append('course%s' % i)
        header_line = utils.cols_to_line(header_cols)
    
    # write to file
    with open(csv, 'a') as f:
        f.write(header_line)
        f.write(utils.cols_to_line(cols))
        f.close()
    
    # get a thank-you message to the user
    return redirect('/thanks/')

def thanks(req):
    ''' displays a simple "thanks for signing up" page '''
    return render(req, 'thanks.html')

def chpassword(req):
    ''' simple password change form, rerendering and functionality '''
    form = req.POST
    username = form.get('username')
    pw_current = form.get('pw_current')
    pw_new = form.get('pw_new')
    pw_newrep = form.get('pw_newrep')
    
    # authenticate
    if username and pw_current:
        user = authenticate(username=username, password=pw_current)
        if user is None or not user.is_active:
            return render(req, 'chpassword.html', {'message': 'incorrect username or password'})
    else:
        return render(req, 'chpassword.html')
    
    # new password consistency
    if pw_new != pw_newrep:
        return render(req, 'chpassword.html', {'message': 'the new password must match its repetition', 'username': username, 'password': pw_current})
    
    # apply password change or show an error message
    try:
        ldap_chpassword.ldap_chpassword(MCWEB_LDAP_DN, 'secretpassword', username, pw_current, pw_new, sudo=False)
        return HttpResponse('Your password has been changed.')

    except Exception as e:
        print(e.message)
        return render(req, 'chpassword.html', {'message': 'your password could not be changed (%s)' % e.message})


####################################################################################################################################
# BELOW: implementations of the views used by the 3 user management pages - login_um.html, userlist_um.html and userdetail_um.html #
####################################################################################################################################


def login_au(req):
    ''' login and check for superuser status '''
    form = req.POST
    
    username = form.get('username', '')
    password = form.get('password', '')
    
    
    if not username or not password:
        return render(req, 'login_au.html')
    
    user = authenticate(username=username, password=password)
    if user is None or not user.is_active or not user.is_superuser:
        return render(req, 'login_au.html')
    
    login(req, user)
    req.session['ldap_password'] = form.get('ldap_password', '')

    return redirect('userlist_au')

def signup_au(req):
    '''  '''
    return render(req, 'signup_au.html', {'courses': settings.COURSES})

def signup_au_get(req):
    ''' handles signup form submission. Add a new signup instance to the db using utils.create_signup '''
    form = req.GET
    
    # get static fields
    firstname = form.get('firstname')
    lastname = form.get('lastname')
    username = form.get('username')
    email = form.get('email')
    
    courses = []
    for c in settings.COURSES:
        if form.get(c):
            courses.append(form.get(c))
    for c in settings.COURSES_MANDATORY:
        courses.append(c)
    
    utils.create_signup(firstname, lastname, email, username, courses)
        
    # get a thank-you message to the user
    return redirect('/thanks/')

@login_required
def userlist_au(req, listtype='new'):
    ''' list all new signups, added users or limbo-users '''
    
    class Ci:
        ''' Cell info data object '''
        def __init__(self, data, cbx=None, btn=None, lbl=None):
            self.cbx = cbx
            self.btn = btn
            self.lbl = lbl
            if cbx is None and btn is None:
                self.lbl = True
            self.data = data
    
    (headers, num_non_course) = utils.get_colheaders()
    colheaders = []
    i = 0
    for colheader in headers:
        i += 1
        #cbx = i > num_non_course
        cbx = None
        colheaders.append(Ci(colheader, cbx))
    
    rows_ids = []
    ids = []
    
    # filter signup object set (and ui state variables message and buttonstate) based on action parameter
    buttondisplay = ''
    if listtype == 'new':
        signups = utils.get_signups()
        message = 'New signups:'
    elif listtype == 'added':
        signups = utils.get_signups_added()
        message = 'Added:'
        buttondisplay = 'none'
    elif listtype == 'limbo':
        signups = utils.get_signups_limbo()
        message = 'Limbo signups - edit to fix errors, then re-submit:'
    else: 
        raise Exception('signupper.views.userlist_au: undefined action')
    
    for s in signups:
        ids.append(s.id)
        
        row = []
        
        row.append(Ci(s.created.strftime("%Y%m%d")))
        row.append(Ci(s.firstname))
        row.append(Ci(s.lastname))
        row.append(Ci(s.email))
        row.append(Ci(s.username))
        
        # courses columns - new/limbo or added signup state
        for course in settings.COURSES + settings.COURSES_MANDATORY:
            if not listtype == 'added' and not s.added_moodle:
                if course in s.courses:
                    row.append(Ci(course, cbx='checked'))
                else:
                    row.append(Ci(course, cbx='unchecked'))
            else:
                if course in s.courses:
                    row.append(Ci('enrolled'))
                else:
                    row.append(Ci(''))
        
        # buttons
        if not listtype == 'added':
            row.append(Ci('edit', btn=True))
            row.append(Ci('delete', btn=True))
        
        # fail string
        if listtype == 'limbo':
            row.append(Ci(s.fail_str))
        
        rows_ids.append([row, str(s.id)])
    
    return render(req, 'userlist_au.html', {'next': '/userlist_au-post', 'ids': ids, 'rows_ids': rows_ids, 'colheaders': colheaders, 'message': message, 'buttondisplay': buttondisplay})

@login_required
def userlist_au_post(req):
    ''' handles list form submission '''
    
    # get filtered signups
    form = req.POST
    ids = ast.literal_eval(form.get('ids')) # conv. str repr. of lst. to lst.
    objs = Signup.objects.filter(id__in=ids)
    
    # get course headers 
    headers, noncourses = utils.get_colheaders()
    courseheaders = headers[noncourses:]
    
    # save new course configuration for each signup
    for s in objs:
        courses = []
        for course in courseheaders:
            # NOTE: non-checked checkboxes do not become included in the form submission
            cbx = form.get('%s_%s' % (str(s.id), course))
            if cbx:
                courses.append(course)
        s.courses = courses
        s.save()
    
    # perform the appropriate add-user actions for each signup
    # NOTE: this algorithm is only fool-proof down to the db save() operation always succeeding
    ldap_password = req.session['ldap_password']
    for s in objs:
        try:
            # try add to ldap
            if not s.added_ldap:
                ldaputils.ldap_adduser()
                s.added_ldap = timezone.now()
                s.save()
            
            # try add to moodle
            if not s.added_moodle:
                utils.moodle_adduser()
                s.added_moodle = timezone.now()
                s.save()
            
            # try notify user
            if not s.notified:
                utils.notifyuser()
                s.notified = timezone.now()
                s.save()
            
            # all three tasks have been completed successfully at some point, mark and save
            s.is_new = False
            s.is_limbo = False
            s.is_added = True
            s.save()
            
        except Exception as e:
            s.fail_str = '%s\n%s' % (s.fail_str, e.__str__())
            s.is_limbo = True
            s.is_new = False
            s.save()
    
    # return to the list 
    if len(utils.get_signups() > 0):
        return redirect('/userlist_au/new')
    elif len(utils.get_signups_limbo() > 0):
        return redirect('/userlist_au/limbo')
    else:
        return redirect('/userlist_au/added')

@login_required
def userlist_au_action(req, action, id):
    ''' handles submitted action - delete or edit '''
    if action == 'delete':
        # TODO: if user has been added to ldap, etc., remove them don't just delete the signup request
        # TODO: make it so that deleted signups are just moved somewhere else, so they may be revived using /admin
        s = Signup.objects.filter(id=int(id))
        s.delete()
        return redirect(userlist_au)
    
    if action == 'edit':
        return redirect('/userdetail_au/%s/' % id)
    
    return HttpResponse('error: unknown action \naction=%s, id=%s' % (action, id))

@login_required
def userdetail_au(req, id):
    ''' change some values in a signup '''
    
    return HttpResponse('id=%s' % (id))
    #return render(req, 'userdetail_au.html')

