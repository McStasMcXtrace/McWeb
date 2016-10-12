'''
Signupper views:

- demo site addusers interface
    - add/enroll individual users
    - signup form (NOTE: the get response is intended to show up in a _blank window.)
    - contact form

- production site course_manage interface:
    - create moodle template from course
    - create course from template, 
    - bulk add/enroll users

'''
from django.shortcuts import render, redirect
from django.contrib.auth import authenticate, login
from django.contrib.auth.decorators import login_required
from django.http import HttpResponse
from django.utils import timezone
from django.core.validators import validate_email

import ast
import os

import utils
from mcweb import settings
from mcweb.settings import MCWEB_LDAP_DN, COURSES, COURSES_MANDATORY
from models import Signup, ContactEntry
from ldaputils import ldaputils
from moodleutils import moodleutils as mu


####################################################################
#                  Demo site addusers section                      #
####################################################################


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

@login_required
def userlist_au(req, listtype='new'):
    ''' list all new signups, added users or limbo-users '''
    
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
    
    # get filtered signups and update
    form = req.POST
    ids = ast.literal_eval(form.get('ids')) # conv. str repr. of lst. to lst.
    objs = Signup.objects.filter(id__in=ids)
    utils.update_signups(objs, form)
    
    # perform the appropriate add-user actions for each signup
    for s in objs:
        utils.adduser(signup, ldap_password=req.session['ldap_password'])
    
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
            
            utils.pull_signups_todb(f, COURSES, COURSES_MANDATORY)
            
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


########################################################################################################
#              Production course manage views - unrelated to the demo addusers functions               #
########################################################################################################


def courseman_login(req):
    ''' login and check for superuser status '''
    form = req.POST
    
    username = form.get('username', '')
    password = form.get('password', '')
    
    if not username or not password:
        return render(req, 'course_login.html')
    
    user = authenticate(username=username, password=password)
    if user is None or not user.is_active or not user.is_superuser:
        return render(req, 'course_login.html')
    
    login(req, user)
    req.session['ldap_password'] = form.get('ldap_password', '')

    return redirect('/coursemanage/users')


def _cmdict(next, message='', dict=None):
    ''' used for course manage rendering '''
    templates_url = '/coursemanage/templates'
    courses_url = '/coursemanage/courses'
    enroll_url = '/coursemanage/users'
    d = {'next' : next, 'message' : message, 'templates_url' : templates_url, 'courses_url' : courses_url, 'enroll_url' : enroll_url}
    if dict != None:
        d.update(dict)
    return d

@login_required
def courseman_templates(req):
    courses = mu.get_courses()
    templates = mu.get_templates()
    
    return render(req, 'course_template.html', _cmdict(next='/coursemanage/templates-post', dict={'courses' : courses, 'templates' : templates}))

@login_required
def courseman_templates_post(req):
    form = req.POST
    
    tmpl = form['course_selector']
    name = form['field_shortname_tmpl']
    
    mu.create_template(name, tmpl)
    
    return HttpResponse('%s, %s' % (tmpl, name))

@login_required
def courseman_courses(req):
    templates = mu.get_templates()
    
    return render(req, 'course_create.html', _cmdict(next='/coursemanage/courses-post', dict={'templates' : templates}))

@login_required
def courseman_courses_post(req):
    '''  '''
    form = req.POST
    
    tmpl = form['tmpl_selector']
    site = form['tbx_site']
    shortname = form['field_shortname']
    title = form['tbx_title']
    
    mu.create_course_from_template(templatename=tmpl, shortname=shortname, fullname=title)
    
    username = form['tbx_username']
    firstname = form['tbx_firstname']
    lastname = form['tbx_lastname']
    email = form['tbx_email']
    
    mu.adduser(firstname=firstname, lastname=lastname, username=username, email=email)

    mu.enroll_user(username=username, course_sn=shortname, teacher=True)
    
    return HttpResponse('%s, %s, %s, %s, %s, %s, %s, %s' % (tmpl, site, shortname, title, username, firstname, lastname, email))

@login_required
def courseman_users_delete(req, id):
    ''' deletes the requested signup object '''
    s = Signup.objects.filter(id=int(id))
    s.delete()
    
    return redirect('/coursemanage/users')

@login_required
def courseman_users(req):
    ''' display the list of signups '''
    message = ''
    
    colheaders = [Ci('date'), Ci('firstname'), Ci('lastname'), Ci('email'), Ci('username'), Ci('password')]
    rows_ids = []
    ids = []
        
    next = '/coursemanage/users-post'
    uploadnext = '/coursemanage/uploadcsv-post'
    
    courses = mu.get_courses()
    
    displaysignups = 'none'
    signups = Signup.objects.filter(is_added=False)
    
    if signups != None:
        displaysignups = ''
        for s in signups:
            row = []
            use_textbox = True
            
            row.append(Ci(s.created.strftime("%Y%m%d")))
            row.append(Ci(s.firstname, txt=use_textbox, header='firstname'))
            row.append(Ci(s.lastname, txt=use_textbox, header='lastname'))
            row.append(Ci(s.email, txt=use_textbox, header='email'))
            row.append(Ci(s.username, txt=use_textbox, header='username'))
            row.append(Ci(s.password, header='passwd'))
            row.append(Ci('delete', btn=True))
            
            rows_ids.append([row, str(s.id)])
            ids.append(s.id)
    
    return render(req, 'course_enroll.html', _cmdict(next='', dict={'colheaders' : colheaders, 'rows_ids' : rows_ids, 'ids' : ids, 'next' : next, 'uploadnext' : uploadnext, 'displaysignups' : displaysignups, 'courses' : courses}))

@login_required
def courseman_users_uploadcsv_post(req):
    ''' handles csv upload - NOTE: the csv-file is not saved to disk '''
    if len(req.FILES) > 0:
        try:
            f = req.FILES['up_file']
            
            # pull csv object to db signup objects 
            utils.pull_signups_todb(f, courses_only=['hest'])
            
        except Exception as e:
            return HttpResponse('Invalid csv file: %s' % e.__str__())
    
    return redirect('/coursemanage/users')

@login_required
def courseman_users_post(req):
    form = req.POST

    # get filtered signups and update
    form = req.POST
    ids = ast.literal_eval(form.get('ids')) # conv. str repr. of lst. to lst.
    signups = Signup.objects.filter(id__in=ids)
    utils.update_signups(signups, form)
    utils.assign_courses(signups, [form['course_selector']] + COURSES_MANDATORY)
    
    # perform the appropriate add-user actions for each signup
    for s in signups:
        #utils.adduser(signup, ldap_password=req.session['ldap_password'])
        utils.adduser(s, ldap_password='pw', accept_ldap_exists=True)
    
    return redirect('/coursemanage/users')


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
