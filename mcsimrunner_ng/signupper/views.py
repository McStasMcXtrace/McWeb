'''
signupper views - NOTE: the get respons is intended to show up in a _blank window, which is why it redirects to 
the thank you page.
'''
from django.shortcuts import render, redirect
from os.path import exists

def signup(req):
    ''' displays the signup form '''
    return render(req, 'signup.html')

def signup_get(req):
    ''' signup form GET parse and save signup to file '''
    form = req.GET
    firstname = form.get('firstname')
    lastname = form.get('lastname')
    username = form.get('username')
    email = form.get('email')
    password = ""
    
    csv = 'new_signups.csv'
    header_str = ""
    if not exists(csv):
        header_str = "firstname;lastname;username;email;password\n"
    
    with open(csv, 'a') as f:
        f.write(header_str)
        f.write('%s;%s;%s;%s;%s\n' % (firstname, lastname, username, email, password))
        f.close()
    
    return redirect('/thanks/')

def thanks(req):
    ''' displays a simple "thanks for signing up" page '''
    return render(req, 'thanks.html')
