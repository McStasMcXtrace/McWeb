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
from django.contrib.auth.decorators import user_passes_test
from django.http import HttpResponse
from django.core.validators import validate_email
from signupper.models import Signup
from django.utils import timezone

import ast
import os
import re

import utils
from mcweb.settings import MCWEB_LDAP_DN, COURSES_MANDATORY, BASE_DIR, FILE_UPLOAD_PW, LDAP_PW
from models import ContactEntry
from ldaputils import ldaputils
from moodleutils import moodleutils as mu
from collections import OrderedDict


####################################################
#                  Demo site                       #
####################################################

def num_signups(req):
    ''' count the total number of signup instances and return this number '''
    all = Signup.objects.all()
    active = [s for s in all if s.state() == 3]
    num_active = len(active)
    return HttpResponse('Status: %d active accounts' % num_active)

def signup(req):
    ''' signup form to be embedded into the wordpress demo site '''
    return render(req, 'signup.html')

def thanks(req):
    ''' displays a simple "thanks for signing up" page '''
    return render(req, 'thanks.html')

def signup_get(req):
    ''' handles signup form submissions '''
    form = req.GET
    
    # get static fields
    firstname = form.get('firstname')
    lastname = form.get('lastname')
    username = form.get('username')
    email = form.get('email')
    
    # get a "username is taken" message to the user
    users = ldaputils.listusers(uid=username)
    if len(users) != 0:
        return HttpResponse('Username %s already exists. Please choose another username and try again.' % username)
    
    signup = Signup(username=username,
                    firstname=firstname,
                    lastname=lastname,
                    email=email,
                    password=utils.get_random_passwd(),
                    courses=COURSES_MANDATORY)
    signup.is_self_signup = True
    signup.save()
    
    # get a thank-you message to the user
    return redirect('/thanks/')

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


##############################################################
#                     Management module                      #
##############################################################

def superlogin(req):
    ''' login and check for superuser status '''
    form = req.POST
    
    username = form.get('username', '')
    password = form.get('password', '')
    
    if not username or not password:
        return render(req, 'man_login.html')
    
    user = authenticate(username=username, password=password)
    if user is None or not user.is_active or not user.is_superuser:
        return render(req, 'man_login.html')
    
    login(req, user)

    return redirect('/manage/')

manage_menu_items = OrderedDict([
                ('signups' , 'Self-signups'),
                ('bulk_add' , 'Bulk Add'),
                ('users' , 'Active Users'),
                ('limbos' , 'Limbos'),
                ('disabled' , 'Disabled'),
                #('deleted' , 'Deleted'),
                ('pause_a' , '---'),
                ('templates' , 'Templates/Backups'),
                ('courses' , 'Course Creation'),
                ('pause_b' , '---'),
                ('upload' , 'Upload files for simulation')])
    
@user_passes_test(lambda u: u.is_superuser)
def manage(req, menu=None, post=None):
    '''  '''
    if not menu in manage_menu_items or menu in ['pause_a', 'pause_b']:
        return redirect('/manage/users')
    
    message = req.session.get('message', '')
    req.session['message'] = ''
    base_context = {'menu' : menu, 'menu_items' : manage_menu_items, 'message' : message}
    
    idx = manage_menu_items.keys().index(menu)
    if idx == 0:
        return man_selfsignups(req, menu, post, base_context)
    elif idx == 1:
        return man_bulk_signup(req, menu, post, base_context)
    elif idx == 2:
        return man_users(req, menu, post, base_context)
    elif idx == 3:
        return man_limbos(req, menu, post, base_context)
    elif idx == 4:
        return man_disabled(req, menu, post, base_context)
    #elif idx == 5:
    #    return man_deleted(req, menu, post, base_context)
    elif idx == 6:
        return man_templates(req, menu, post, base_context)
    elif idx == 7:
        return man_courses(req, menu, post, base_context)
    elif idx == 9:
        return man_upload(req, menu, post, base_context)
    else:
        raise Exception("code inconsistence")

class CellInfo:
    def __init__(self, data, idx, cbx=None, txt=None, lbl=None):
        self.cbx = cbx
        self.txt = txt
        self.lbl = lbl
        if cbx is None and txt is None:
            self.lbl = True
        self.data = data
        self.idx = idx

def man_selfsignups(req, menu, post, base_context):
    if post == 'post':
        # get filtered signups depending on the hidden field "ids" as well as list selection (checkboxes following a naming convention)
        form = req.POST
        ids = ast.literal_eval(form.get('ids')) # conv. str repr. of lst. to lst.
        ids = [i  for i in ids if form.get('%s_cbx' % i) == 'on']
        objs = Signup.objects.filter(id__in=ids)
        
        # update objs with local data (text boxes)
        for signup in objs:
            signup.firstname = form.get('%s_%d' % (str(signup.id), 2))
            signup.lastname = form.get('%s_%d' % (str(signup.id), 3))
            signup.email = form.get('%s_%d' % (str(signup.id), 4))
            signup.username = form.get('%s_%d' % (str(signup.id), 5))
            signup.save()
        
        # get bulk action
        action = form.get('bulk_actions')
        
        if action == 'add_enrol':
            err_flag = False
            for signup in objs:
                utils.adduser(signup)
                for course in signup.courses:
                    error_or_None = mu.enroll_user(signup.username, course)
                    if error_or_None:
                        signup.fail_str = signup.fail_str + ' ' + error_or_None
                if signup.state() != 3:
                    req.session['message'] = 'Some signups reported an error, e.g. %s' % signup.fail_str
                    err_flag = True
            if not err_flag:
                req.session['message'] = 'Signups were added and enroled.'
        elif action == 'delete':
            for signup in objs:
                signup.delete()
            req.session['message'] = 'Selected entries were deleted.'
        
        return redirect('/manage/%s' % menu)
    
    elif post:
        return redirect('/manage/%s' % menu)
    
    # bulk actions for this view
    bulk_actions = []
    bulk_actions.append('add_enrol')
    bulk_actions.append('delete')
    
    signups = [s for s in Signup.objects.all() if s.state() == 2]
    
    rows_ids = []
    ids = []
    if len(signups) > 0:
        for s in signups:
            row = []
            
            row.append(CellInfo(s.created.strftime("%Y%m%d"), 1))
            row.append(CellInfo(s.firstname, 2, txt=True))
            row.append(CellInfo(s.lastname, 3, txt=True))
            row.append(CellInfo(s.email, 4, txt=True))
            row.append(CellInfo(s.username, 5, txt=True))
            row.append(CellInfo(s.password, 6))
            
            rows_ids.append([row, str(s.id)])
            ids.append(s.id)
    
    context = {'next' : '/manage/%s/post' % menu,
               'rows_ids' : rows_ids,
               'ids' : ids,
               'bulk_actions' : bulk_actions,
               'demo_courses_string' : ', '.join(COURSES_MANDATORY)}
    context.update(base_context)
    return render(req, 'man_selfsignups.html', context)

def man_bulk_signup(req, menu, post, base_context):
    if post == 'post':
        # get filtered signups depending on the hidden field "ids" as well as list selection (checkboxes following a naming convention)
        form = req.POST
        ids = ast.literal_eval(form.get('ids')) # conv. str repr. of lst. to lst.
        ids = [i  for i in ids if form.get('%s_cbx' % i) == 'on']
        objs = Signup.objects.filter(id__in=ids)
        
        # update objs with local data (text boxes)
        for signup in objs:
            signup.firstname = form.get('%s_%d' % (str(signup.id), 2))
            signup.lastname = form.get('%s_%d' % (str(signup.id), 3))
            signup.email = form.get('%s_%d' % (str(signup.id), 4))
            signup.username = form.get('%s_%d' % (str(signup.id), 5))
            signup.save()
        
        # get bulk action
        action = form.get('bulk_actions')
        
        if action == 'add':
            err_flag = False
            for signup in objs:
                utils.adduser(signup)
                if signup.state() != 3:
                    req.session['message'] = 'Some signups reported an error, e.g. %s' % signup.fail_str
                    err_flag = True
            if not err_flag:
                req.session['message'] = 'Selected signups were added.'
        
        elif action == 'delete':
            for signup in objs:
                signup.delete()
            req.session['message'] = 'Selected signups were deleted.'
        elif re.match('add_enroll_', action):
            err_flag = False
            course = re.match('add_enroll_(.*)', action).group(1)
            for signup in objs:
                utils.adduser(signup)
                error_or_None = mu.enroll_user(signup.username, course)
                if error_or_None:
                    signup.fail_str = signup.fail_str + ' ' + error_or_None
                    signup.save()
                if signup.state() != 3:
                    req.session['message'] = 'Some signups reported an error, e.g. %s' % signup.fail_str
                    err_flag = True
            if not err_flag:
                req.session['message'] = 'Selected signups were added and enroled.'
        
        return redirect('/manage/%s' % menu)
    
    elif post == 'uploadcsv_post':
        if len(req.FILES) > 0:
            try:
                f = req.FILES['up_file']
                utils.pull_csv_signups_todb(f)
            except Exception as e:
                req.session['message'] = 'Bulk add processing error: %s' % e.__str__()
        return redirect('/manage/%s' % menu)

    elif post:
        return redirect('/manage/%s' % menu)
    
    # bulk actions for this view
    bulk_actions = []
    bulk_actions.append('add')
    bulk_actions.append('delete')
    courses = mu.get_courses()
    if len(courses) > 0:
        bulk_actions.append('---')
    for c in courses:
        bulk_actions.append('add_enroll_%s' % c)
    
    signups = [s for s in Signup.objects.all() if s.state() == 1]
    
    rows_ids = []
    ids = []
    if len(signups) > 0:
        for s in signups:
            row = []
            
            row.append(CellInfo(s.created.strftime("%Y%m%d"), 1))
            row.append(CellInfo(s.firstname, 2, txt=True))
            row.append(CellInfo(s.lastname, 3, txt=True))
            row.append(CellInfo(s.email, 4, txt=True))
            row.append(CellInfo(s.username, 5, txt=True))
            row.append(CellInfo(s.password, 6))
            
            rows_ids.append([row, str(s.id)])
            ids.append(s.id)
    
    context = {'next' : '/manage/%s/post' % menu,
               'uploadnext' : '/manage/%s/uploadcsv_post' % menu,
               'rows_ids' : rows_ids,
               'ids' : ids,
               'courses' : courses,
               'bulk_actions' : bulk_actions}
    context.update(base_context)
    return render(req, 'man_bulk.html', context)

def man_users(req, menu, post, base_context):

    if post=='post':
        # get filtered signups depending on the hidden field "ids" as well as list selection (checkboxes following a naming convention)
        form = req.POST
        ids = ast.literal_eval(form.get('ids')) # conv. str repr. of lst. to lst.
        ids = [i  for i in ids if form.get('%s_cbx' % i) == 'on']
        objs = Signup.objects.filter(id__in=ids)
        
        # get bulk action
        action = form.get('bulk_actions')
        
        if action == 'disable':
            for signup in objs:
                ldaputils.rmuser(MCWEB_LDAP_DN, LDAP_PW, signup.username)
                signup.is_in_ldap = False
                signup.password = ''
                signup.save()
                req.session['message'] = 'Selected signups were disabled.'
        elif re.match('enroll_', action):
            course = re.match('enroll_(.*)', action).group(1)
            err_flag = False
            for signup in objs:
                error_or_None = mu.enroll_user(signup.username, course)
                if error_or_None:
                    signup.fail_str = signup.fail_str + ' ' + error_or_None
                if signup.state() != 3:
                    req.session['message'] = 'Some signups reported an error, e.g. %s' % signup.fail_str
                    err_flag = True
            if not err_flag:
                req.session['message'] = 'Selected signups were enroled.'
                
        return redirect("/manage/%s" % menu)
    
    # bulk actions for this view
    bulk_actions = []
    bulk_actions.append('disable')
    courses = mu.get_courses()
    if len(courses) > 0:
        bulk_actions.append('---')
    for c in courses:
        bulk_actions.append('enroll_%s' % c)
    
    # filter signups
    signups = [s for s in Signup.objects.all() if s.state() == 3]
    
    rows_ids = []
    ids = []
    for s in signups:
        ids.append(s.id)
        row = []
        
        row.append(CellInfo(s.created.strftime("%Y%m%d"), 1))
        row.append(CellInfo(s.firstname, 2))
        row.append(CellInfo(s.lastname, 3))
        row.append(CellInfo(s.email, 4))
        row.append(CellInfo(s.username, 5))
        row.append(CellInfo(s.password, 6))
        
        rows_ids.append([row, str(s.id)])
    
    context = {'next': '/manage/%s/post' % menu, 'uploadnext': '/manage/%s/upload' % menu,
               'ids': ids, 'rows_ids': rows_ids,
               'bulk_actions' : bulk_actions}
    context.update(base_context)
    return render(req, 'man_users.html', context)

def man_limbos(req, menu, post, base_context):
    '''  '''
    if post=='post':
        # get filtered signups depending on the hidden field "ids" as well as list selection (checkboxes following a naming convention)
        form = req.POST
        ids = ast.literal_eval(form.get('ids')) # conv. str repr. of lst. to lst.
        ids = [i  for i in ids if form.get('%s_cbx' % i) == 'on']
        objs = Signup.objects.filter(id__in=ids)
        
        # update objs with local data (text boxes)
        for signup in objs:
            signup.firstname = form.get('%s_%d' % (str(signup.id), 2))
            signup.lastname = form.get('%s_%d' % (str(signup.id), 3))
            signup.email = form.get('%s_%d' % (str(signup.id), 4))
            signup.username = form.get('%s_%d' % (str(signup.id), 5))
            signup.save()
        
        # get bulk action
        action = form.get('bulk_actions')
        
        if action == 'add':
            for signup in objs:
                utils.adduser(signup)
                
            req.session['message'] = 'Signups were attempted re-added, and were notified if this went well.'
        elif action == 'delete':
            for signup in objs:
                signup.delete()
            req.session['message'] = 'Signups were deleted.'
        
        
        return redirect("/manage/%s" % menu)
    
    # bulk actions for this view
    bulk_actions = []
    bulk_actions.append('add')
    bulk_actions.append('delete')
    
    # filter signups
    signups = [s for s in Signup.objects.all() if s.state() == 0]
    
    rows_ids = []
    ids = []
    for s in signups:
        ids.append(s.id)
        row = []
        
        row.append(CellInfo(s.created.strftime("%Y%m%d"), 1))
        row.append(CellInfo(s.firstname, 2))
        row.append(CellInfo(s.lastname, 3))
        row.append(CellInfo(s.email, 4))
        row.append(CellInfo(s.username, 5))
        row.append(CellInfo(s.password, 6))
        
        rows_ids.append([row, str(s.id)])
    
    context = {'next': '/manage/%s/post' % menu, 'uploadnext': '/manage/%s/upload' % menu,
               'ids': ids, 'rows_ids': rows_ids,
               'bulk_actions' : bulk_actions}
    context.update(base_context)
    return render(req, 'man_users.html', context)

def man_disabled(req, menu, post, base_context):
    '''  '''
    if post=='post':
        # get filtered signups depending on the hidden field "ids" as well as list selection (checkboxes following a naming convention)
        form = req.POST
        ids = ast.literal_eval(form.get('ids')) # conv. str repr. of lst. to lst.
        ids = [i  for i in ids if form.get('%s_cbx' % i) == 'on']
        objs = Signup.objects.filter(id__in=ids)
        
        # get bulk action
        action = form.get('bulk_actions')
        
        if action == 'activate_and_notify':
            for signup in objs:
                signup.password = utils.get_random_passwd()
                ldaputils.addsignup(signup)
                utils.notify_signup(signup)
                
            req.session['message'] = 'Selected signups were re-activated and notified.'
        
        return redirect("/manage/%s" % menu)
    
    # bulk actions for this view
    bulk_actions = []
    bulk_actions.append('activate_and_notify')
    
    # filter signups
    signups = [s for s in Signup.objects.all() if s.state() == 5]
    
    rows_ids = []
    ids = []
    for s in signups:
        ids.append(s.id)
        row = []
        
        row.append(CellInfo(s.created.strftime("%Y%m%d"), 1))
        row.append(CellInfo(s.firstname, 2))
        row.append(CellInfo(s.lastname, 3))
        row.append(CellInfo(s.email, 4))
        row.append(CellInfo(s.username, 5))
        row.append(CellInfo(s.password, 6))
        
        rows_ids.append([row, str(s.id)])
    
    context = {'next': '/manage/%s/post' % menu, 'uploadnext': '/manage/%s/upload' % menu,
               'ids': ids, 'rows_ids': rows_ids,
               'bulk_actions' : bulk_actions}
    context.update(base_context)
    return render(req, 'man_users.html', context)

def man_deleted(req, menu, post, base_context):
    context = {}
    context.update(base_context)
    return render(req, 'man_deleted.html', context)

def man_templates(req, menu, post, base_context):
    '''  '''
    if post == 'post':
        form = req.POST
        shortname = form['course_selector']
        tmplname =  form['field_shortname_tmpl']
        m = re.match('\-\-\sselect\sfrom', shortname)
        if tmplname != '' and not m:
            mu.create_template(shortname, tmplname)
            req.session['message'] = 'Template %s created from %s.' % (tmplname, shortname)
        else:
            req.session['message'] = 'Please select a proper course and a template name.'
        return redirect("/manage/%s" % menu)
    elif post:
        return redirect("/manage/%s" % menu)
    
    courses = mu.get_courses()
    templates = mu.get_templates()
    
    context = {'courses' : courses, 'templates' : templates, 'next' : '/manage/%s/post' % menu}
    context.update(base_context)
    return render(req, 'man_templates.html', context)


def man_courses(req, menu, post, base_context):
    '''  '''
    if post == 'post':
        form = req.POST
        
        tmpl = form['tmpl_selector']
        site = form['tbx_site']
        shortname = form['field_shortname']
        title = form['tbx_title']
        
        m = re.match('\-\-\sselect\sfrom', shortname)
        if site != '' and shortname != '' and title != '' and not m:
            (backupname, courseid, status) = mu.create_course_from_template(templatename=tmpl, shortname=shortname, fullname=title)
        else:
            req.session['message'] = 'Please select a proper template and a course name.'
            return redirect("/manage/%s" % menu)
        
        username = form['tbx_username']
        firstname = form['tbx_firstname']
        lastname = form['tbx_lastname']
        email = form['tbx_email']
        
        # double-check that a user has been selected
        if username == '':
            req.session['message'] = 'Please assign a teacher for the course.'
            return redirect("/manage/%s" % menu)
        
        users = ldaputils.listusers(uid=username)
        if firstname != '' or email != '':
            if len(users) != 0:
                t = users[0]
                req.session['message'] = 'User %s already exists with "%s %s, %s", please clear the name and email fields.' % (username, t.cn, t.sn, t.mail)
                return redirect("/manage/%s" % menu)
            
            if firstname == '' or email == '':
                req.session['message'] = 'New user creation requires a name and an email.'
                return redirect("/manage/%s" % menu)
            teacher = Signup(username=username, firstname=firstname, lastname=lastname, email=email, password=utils.get_random_passwd(), courses=[shortname])
            teacher.save()
            
            utils.adduser(teacher)
            error_or_None = mu.enroll_user(username=username, course_sn=shortname, teacher=True)
            
            req.session['message'] = 'New user %s has been created.' % username
        elif len(users) and username == users[0].uid:
            error_or_None = mu.enroll_user(username=username, course_sn=shortname, teacher=True)
        else:
            # username does not exist, but user did not enter name etc.
            req.session['message'] = 'Please enter the name and email of the teacher of this course.'
            return redirect("/manage/%s" % menu)
        
        
        if error_or_None:
            teacher.fail_str = teacher.fail_str + ' ' + error_or_None
            raise Exception('Teacher could not be added: %s') % error_or_None
            
        req.session['message'] = '\n'.join([req.session['message'], 'Course %s created with teacher %s. Restoring contents in the background...' % (shortname, username)])
        
        return redirect("/manage/%s" % menu)
    
    elif post:
        return redirect("/manage/%s" % menu)
    
    context = {'templates' : mu.get_templates(),  'next' : '/manage/%s/post' % menu}
    context.update(base_context)
    return render(req, 'man_courses.html', context)

def man_upload(req, menu, post, base_context):
    '''  '''
    def group_dirs():
        for _, dirs, _ in os.walk('sim/'):
            # stackoverflow-ish alphanum sort 
            dirs = sorted(dirs, key=lambda item: (int(item.partition(' ')[0])
                                   if item[0].isdigit() else float('inf'), item))
            return dirs
    
    def shepherd(file_obj, g):
        file_path = "%s/%s"%(os.path.join(BASE_DIR, 'sim', g), file_obj.name)
        with open(file_path, 'wb+') as out_file:
            for chunk in file_obj.chunks():
                out_file.write(chunk)
        return "saved to: %s" % file_path
    
    if post == 'post':
        form = req.POST
        if not form:
            return redirect('/manage/%s' % menu)
        
        # check password
        pw = form['password']
        req.session['man_upload_pw'] = pw
        if pw == '':
            return redirect('/manage/%s' % menu)
        if pw != FILE_UPLOAD_PW:
            req.session['message'] = 'Please enter the correct password'
            return redirect('/manage/%s' % menu)
        
        # handle form
        g = form['groupdir']
        
        # mkdir if required (and set g to new group_dir)...
        d = form['new_group']
        if d != '' and d not in group_dirs():
            os.mkdir('sim/%s' % d)
            req.session['message'] = 'Group dir %s was created.' % d
            g = d
        elif g == "select_a_group":
            req.session['message'] = 'Please select or create a group dir.'
            return redirect('/manage/%s' % menu)
        
        # get file
        if len(req.FILES) > 0:
            f = req.FILES['up_file']
            shepherd(f, g)
            req.session['message'] = 'File %s uploaded to %s.' % (f.name, g)
        else:
            req.session['message'] = 'Please select a file for upload.'
            return redirect('/manage/%s' % menu)
        
        return redirect('/manage/%s' % menu)

    groupdirs = group_dirs()    
    pw = req.session.get('man_upload_pw', '')
    
    context = {'groupdirs': groupdirs, 'password': '', 'password': pw,
               'next' : '/manage/%s/post' % menu
               }
    context.update(base_context)
    return render(req, 'man_upload.html', context)

