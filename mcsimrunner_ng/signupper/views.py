'''
signupper views - NOTE: the get response is intended to show up in a _blank window, which is why it redirects to 
the thank you page.
'''
from django.shortcuts import render, redirect
from os.path import exists
from subprocess import Popen, PIPE
from mcweb import settings

def signup(req):
    ''' displays the signup form '''
    return render(req, 'signup.html', {'courses': settings.COURSES})

def get_random_passwd():
    try:
        process = Popen('makepasswd', stdout=PIPE, stderr=PIPE, shell=True)
        (stdoutdata, stderrdata) = process.communicate()
        return stdoutdata.strip()
    except:
        print 'get_random_passwd() --> Popen("makepasswd") output:\n' + stderrdata
        raise

def cols_to_line(cols):
    ''' makes a list of strings into a single ;-separated string with a trailing linebreak '''
    line = ""
    for c in cols:
        line = "%s%s;" % (line, c)
    return line.rstrip(";") + "\n"

def signup_get(req):
    ''' signup form GET parsing, and append the signup line to file new_signups.csv '''
    
    # introduce the file
    csv = 'new_signups.csv'
    
    # and the form
    form = req.GET
    
    # get static fields from the form
    firstname = form.get('firstname')
    lastname = form.get('lastname')
    username = form.get('username')
    email = form.get('email')
    password = get_random_passwd()
    cols = [firstname, lastname, username, email, password]
    
    # get dynamic fields from the form
    for c in settings.COURSES:
        cols.append(str(bool(form.get(c))))
    for c in settings.COURSES_MANDATORY:
        cols.append(str(True))
    
    # create the header line - an empty string if the file exists
    header_line = ""
    if not exists(csv):
        header_cols = ["firstname", "lastname", "username", "email", "password"]
        for c in settings.COURSES:
                header_cols.append(c)
        for c in settings.COURSES_MANDATORY:
                header_cols.append(c)
        header_line = cols_to_line(header_cols)
    
    # write to file
    with open(csv, 'a') as f:
        f.write(header_line)
        f.write(cols_to_line(cols))
        f.close()
    
    # get a thank-you message to the user
    return redirect('/thanks/')

def thanks(req):
    ''' displays a simple "thanks for signing up" page '''
    return render(req, 'thanks.html')
