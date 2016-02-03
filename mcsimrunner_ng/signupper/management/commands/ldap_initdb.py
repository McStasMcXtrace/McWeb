'''
A command to make it easy calling the function ldaputils.initdb(), to enter the ldap admin password and the sudo password.
'''
from django.core.management.base import BaseCommand
from mcweb.settings import MCWEB_LDAP_DN
from signupper.ldaputils.ldaputils import ldap_initdb

class Command(BaseCommand):
    ''' Django ldap init db command for mcweb. '''
    help = 'Inits  the mcweb ldap db. Must be called once before users can be added. '

    def add_arguments(self, parser):
        ''' pass the ldap admin password to this command '''
        #parser.add_argument('password', nargs=1, action='store_true', help="ldap admin password")
    
        parser.add_argument('password', nargs=1, type=str, help='ldap admin password')
        
    def handle(self, *args, **options):
        ''' Implements Django ldap init db command execution. '''
        ldap_initdb(MCWEB_LDAP_DN, options['password'][0])
