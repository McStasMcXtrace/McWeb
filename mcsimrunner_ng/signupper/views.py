'''
signupper views - NOTE: the get response is intended to show up in a _blank window, which is why it redirects to 
the thank you page.
'''
from django.shortcuts import render, redirect
from django.contrib.auth import authenticate, login
from django.contrib.auth.decorators import login_required
from django.http import HttpResponse
from django.utils import timezone
from django.core.validators import validate_email

import ast
import os
import csv

import utils
from mcweb import settings
from mcweb.settings import MCWEB_LDAP_DN, COURSES, COURSES_MANDATORY
from models import Signup, ContactEntry
from ldaputils import ldaputils
from moodleutils import moodleutils


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
    ''' signup form to be embedded '''
    return render(req, 'signup_au.html', {'courses': settings.COURSES})

def thanks(req):
    ''' displays a simple "thanks for signing up" page '''
    return render(req, 'thanks.html')

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
        def __init__(self, data, header=None, cbx=None, btn=None, txt=None, lbl=None):
            self.cbx = cbx
            self.btn = btn
            self.txt = txt
            self.lbl = lbl
            if cbx is None and btn is None and txt is None:
                self.lbl = True
            self.data = data
            self.header = header
    
    headers, num_non_course = utils.get_colheaders()
    colheaders = []
    i = 0
    for colheader in headers:
        i += 1
        cbx = None
        colheaders.append(Ci(colheader, cbx))
    
    rows_ids = []
    ids = []
    
    # filter signup object set (and ui state variables message and buttonstate) based on action parameter
    buttondisplay = ''
    uploaddisplay = ''
    if listtype == 'new':
        signups = utils.get_signups()
        message = 'New signups:'
    elif listtype == 'added':
        signups = utils.get_signups_added()
        message = 'Added:'
        buttondisplay = 'none'
        uploaddisplay = 'none'
    elif listtype == 'limbo':
        signups = utils.get_signups_limbo()
        message = 'Limbo signups - edit to fix errors, then re-submit:'
        uploaddisplay = 'none'
    else: 
        raise Exception('signupper.views.userlist_au: undefined action')
    
    for s in signups:
        ids.append(s.id)
        
        row = []
        use_textbox = not s.added_ldap
        row.append(Ci(s.created.strftime("%Y%m%d")))
        row.append(Ci(s.firstname, txt=use_textbox, header='firstname'))
        row.append(Ci(s.lastname, txt=use_textbox, header='lastname'))
        row.append(Ci(s.email, txt=use_textbox, header='email'))
        row.append(Ci(s.username, txt=use_textbox, header='username'))
        row.append(Ci(s.password, header='passwd'))
        
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
        if listtype == 'new':
            #row.append(Ci('edit', btn=True))
            row.append(Ci('delete', btn=True))
        
        # fail string
        if listtype == 'limbo':
            row.append(Ci(s.fail_str))
        
        rows_ids.append([row, str(s.id)])
    
    return render(req, 'userlist_au.html', {'next': '/userlist_au-post', 'uploadnext': '/upload_au_post', 'ids': ids, 'rows_ids': rows_ids, 'colheaders': colheaders, 
                                            'message': message, 'buttondisplay': buttondisplay, 'uploaddisplay': uploaddisplay})

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
    
    # get and save new configuration for each signup
    for s in objs:
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
    
    # perform the appropriate add-user actions for each signup
    # NOTE: this algorithm is only fool-proof down to the db save() operation succeeding
    ldap_password = req.session['ldap_password']
    for s in objs:
        try:
            # try add to ldap
            if not s.added_ldap:
                ldaputils.ldap_adduser(MCWEB_LDAP_DN, ldap_password, s.firstname, s.lastname, s.username, s.email, s.password)
                s.added_ldap = timezone.now()
                s.save()
            
            # try add to moodle
            if not s.added_moodle:
                moodleutils.add_enroll_user(s.firstname, s.lastname, s.username, s.email, s.courses)
                s.added_moodle = timezone.now()
                s.save()
            
            # try notify user
            if not s.notified:
                utils.notifyuser(s.firstname + ' ' + s.lastname, s.username, s.email, s.password)
                s.notified = timezone.now()
                s.save()
            
            # all three tasks have been completed successfully at some point, mark and save
            s.is_new = False
            s.is_limbo = False
            s.is_added = True
            s.fail_str = ''
            s.save()
            
        except Exception as e:
            s.fail_str = '%s\n%s' % (s.fail_str, e.__str__())
            s.is_limbo = True
            s.is_new = False
            s.save()
    
    # return to the list 
    if len(utils.get_signups()) > 0:
        return redirect('/userlist_au/new')
    elif len(utils.get_signups_limbo()) > 0:
        return redirect('/userlist_au/limbo')
    else:
        return redirect('/userlist_au/added')

@login_required
def userlist_au_action(req, action, signup_id):
    ''' handles submitted action - delete or edit '''
    if action == 'delete':
        # TODO: if user has been added to ldap, etc., remove them don't just delete the signup request
        # TODO: make it so that deleted signups are just moved somewhere else, so they may be revived using /admin
        s = Signup.objects.filter(id=int(signup_id))
        s.delete()
        return redirect(userlist_au)
    
    if action == 'edit':
        return redirect('/userdetail_au/%s/' % signup_id)
    
    return HttpResponse('error: unknown action \naction=%s, id=%s' % (action, signup_id))

@login_required
def upload_au_post(req):
    ''' handles csv upload - NOTE: the csv-file is not saved to disk '''
    if len(req.FILES) > 0:
        try:
            f = req.FILES['up_file']
            r = csv.reader(f, delimiter=',')
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
                
                # match courses in row
                courses = []
                for known in COURSES:
                    if known in row:
                        courses.append(known)
                courses = courses + COURSES_MANDATORY
                
                utils.create_signup(row[firstname_idx], row[lastname_idx], row[email_idx], row[username_idx], courses)
        except Exception as e:
            return HttpResponse('Invalid csv file: %s' % e.__str__())
    
    return redirect('/userlist_au/new')

@login_required
def userdetail_au(req, id):
    ''' change some values in a signup '''
    
    return HttpResponse('id=%s' % (id))
    #return render(req, 'userdetail_au.html')

@login_required
def chpassword(req):
    ''' A simple password change form, but requires an "admin-tool" session with the ldap admin password. '''
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
        return render(req, 'chpassword.html', {'message': 'the new password must match its repetition', 'username': username, 
                                               'password': pw_current})
    
    # apply password change or show error
    try:
        ldap_admin_pw = req.session['ldap_password']
        ldaputils.ldap_chpassword(MCWEB_LDAP_DN, ldap_admin_pw, username, pw_current, pw_new)
        return HttpResponse('Your password has been changed.')

    except Exception as e:
        print(e.message)
        return render(req, 'chpassword.html', {'message': 'your password could not be changed (%s)' % e.message})


####################################################
#                  Contact form                    #
####################################################

def contact(req):
    ''' contact form rendering and submission '''
    form = req.POST
    replyto = form.get('replyto', '')
    text = form.get('text', '')
    
    # render empty contact form
    if replyto == '' and text == '':
        return render(req, 'contact.html', {'message' : ''})

    # handle contact entry    
    entry = None
    try: 
        # validate
        validate_email(replyto)
        if text == '':
            raise Exception()
        
        entry = ContactEntry(replyto=replyto, text=text)
        try:
            utils.notify_contactentry(entry.replyto, entry.text)
            entry.delivered = timezone.now()
        except Exception as e:
            entry.fail_str = e.message
            
        entry.save()
        
        return render(req, 'contact.html', {'message' : 'Thank you for submitting your message.'})

    except Exception as e:
        return render(req, 'contact.html', {'message' : 'Fail: %s' % e.message})


####################################################
#                  Deprecated                      #
####################################################

#@deprecated
def signup(req):
    ''' displays the signup form '''
    return render(req, 'signup.html', {'courses': settings.COURSES})

#@deprecated
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
