'''
Generates and prints a signup form with a checkbox for every entry in mcweb.settings.COURSES.
Uses signup.html from signupper as the template, thus changes to this file may break this command.
This is an easy way of updating the signup embedded in webpages, when updating the COURSES list.
'''
from django.core.management.base import BaseCommand
from django.template import Context
from django.template.loader import get_template
from mcweb import settings
from re import search

class Command(BaseCommand):
    help = 'prints a signup form based on mcweb.settings.COURSES and signuppper.templates.signup.html'
    
    def handle(self, *args, **options):
        t = get_template('signup.html')
        c = Context({'courses': settings.COURSES})
        html = t.render(c)
        form = search(r'<form[\w\s<>/="-:]*</form>', html).group()
        print '    ' +form
