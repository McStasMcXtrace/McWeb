'''
Lists db instrument objects.
'''
from django.core.management.base import BaseCommand, CommandError
from signupper.models import Signup
from django.contrib.auth.models import User

class Command(BaseCommand):
    help = 'list instrument objects currently in the django db'
    def handle(self, *args, **options):

        l = Signup.objects.all()
        print("# displaying %d registered User objects..." % len(l))
        for i in l:
            print(i.username + " " + i.email)


        
