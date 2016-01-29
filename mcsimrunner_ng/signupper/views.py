'''
signupper views - NOTE: the get response is intended to show up in a _blank window, which is why it redirects to 
the thank you page.
'''
from django.shortcuts import render, redirect
from os.path import exists
from django.contrib.auth import authenticate, login
from django.http import HttpResponse

from useradmin import ldap_chpassword

from mcweb import settings
from mcweb.settings import MCWEB_LDAP_DN

import utils

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
    if not exists(csv):
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


########################################
# BELOW are the views used by the 3 user management pages - login_um.html, userlist_um.html and userdetail_um.html

def login_au(req):
    ''' login and check for superuser status '''
    form = req.POST
    
    username = form.get('username', '')
    password = form.get('password', '')
    
    if not username or not password:
        return render(req, 'login_au.html', {'next': '/userlist_au'})
    
    user = authenticate(username=username, password=password)
    if user is None or not user.is_active or not user.is_superuser:
        return render(req, 'login_au.html', {'next': '/userlist_au'})
    
    login(req, user)

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


def userlist_au(req):
    ''' list all new signups, added users or limbo-users '''
    class Ci:
        ''' Cell info data object. Either cbx or lbl is true, the other is None '''
        def __init__(self, data, cbx=False):
            if cbx:
                self.cbx = True
            else:
                self.lbl = True
            self.data = data
    
    #
    (headers, num_non_course) = utils.get_moodle_colheaders()
    colheaders = []
    i = 0
    for colheader in headers:
        i += 1
        cbx = i > num_non_course
        colheaders.append(Ci(colheader, cbx))
    
    #colheaders = [Ci('header1'), Ci('header2'), Ci('header3'), Ci('cbx_label', cbx=True)]
    rowcolstrings = [[Ci('r0c0'),Ci('r0c1'),Ci('r0c2'),Ci('r0c3')],
                     [Ci('r1c0'),Ci('r1c1'),Ci('r1c2'),Ci('r1c3')],
                     [Ci('r2c0'),Ci('r2c1'),Ci('r2c2'),Ci('r2c3')],
                     [Ci('r3c0'),Ci('r3c1'),Ci('r3c2'),Ci('r3c3')],
                     ]
    
    return render(req, 'userlist_au.html', {'rowcolstrings': rowcolstrings, 'colheaders': colheaders})


def userdetail_au(req):
    ''' change some values in a signup '''
    return render(req, 'userdetail_au.html')
