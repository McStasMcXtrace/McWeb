'''
A command for accessing ldaputils.rmuser.
'''
from django.core.management.base import BaseCommand
from mcweb.settings import MCWEB_LDAP_DN
from signupper.ldaputils.ldaputils import rmuser

class Command(BaseCommand):
    ''' ldap rmuser. '''
    help = 'Removes user entry from the mcweb-configured LDAP db. '

    def add_arguments(self, parser):
        ''' pass the ldap admin password to this command '''
        parser.add_argument('admin_password', nargs=1, type=str, help='ldap admin password')
        parser.add_argument('uid', nargs=1, type=str, help='uid (username) of user to be removed')
        
    def handle(self, *args, **options):
        ''' Implements the django rmuser command '''
        uid = options['uid'][0]
        admin_password = options['admin_password'][0]
        try:
            rmuser(MCWEB_LDAP_DN, admin_password, uid)
            print('%s has been removed from the mcweb LDAP db' % uid)
        except Exception as e:
            print('%s could not be removed: %s' % (uid, e.__str__()))
        