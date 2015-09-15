from django.core.management.base import BaseCommand
from django.contrib.auth.models import User, Group
from getpass import getpass

import sys

class Command(BaseCommand):
    def add_arguments(self, parser):
        parser.add_argument('username', nargs=1, type=str)
        parser.add_argument('password', nargs=1, type=str)
<<<<<<< HEAD
	parser.add_argument('group', nargs='+', type=str)
=======
        parser.add_argument('group', nargs='*', type=str)
>>>>>>> 141c78faf9b295f63fc8863f6778fdb0bb403d24

    def handle(self, *args, **options):
        if len(options) < 1:
            print 'usage: %s username [password]' % options[0]
            sys.exit(1)
    
        username = options['username']
        print 'Username:', username
    
        if User.objects.filter(username=username).count() > 0:
            print 'Abort: User already exists!'
            sys.exit(1)
    
        if len(options) > 1:
            print 'Password taken from argument'
            password = options['password']
        else:
            password = getpass('Enter password: ')
        
        user = User.objects.create_user(username, password=password)
        user.save()
        
        if len(options) > 2:
            for idx in range(0,len(options['group'])):
                print '- adding user to group '+options['group'][idx]
                try:
                    g = Group.objects.get(name=options['group'][idx]) 
                    g.user_set.add(user)
                except:
                    print 'Error: Group '+options['group'][idx]+' does not exist!'
    
        print 'User created.'
