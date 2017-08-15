'''
A command for accessing ldaputils.adduser.
'''
from django.core.management.base import BaseCommand
from mcweb.settings import MCWEB_LDAP_DN
from signupper.ldaputils.ldaputils import adduser

class Command(BaseCommand):
    ''' ldap adduser. '''
    help = 'Adds a user entry to the mcweb-configured LDAP db. '

    def add_arguments(self, parser):
        # adduser(dn, admin_password, cn, sn, uid, email, pw)
        parser.add_argument('admin_password', nargs=1, type=str, help='ldap admin password')
        parser.add_argument('cn', nargs=1, type=str, help='name')
        parser.add_argument('sn', nargs=1, type=str, help='sirname')
        parser.add_argument('uid', nargs=1, type=str, help='uid (username) of user to be added')
        parser.add_argument('email', nargs=1, type=str, help='user email')
        parser.add_argument('pw', nargs=1, type=str, help='user password')

    def handle(self, *args, **options):
        ''' Implements the django ldap adduser command '''
        admin_password = options['admin_password'][0]
        cn = options['cn'][0]
        sn = options['sn'][0]
        uid = options['uid'][0]
        email = options['email'][0]
        pw = options['pw'][0]
        try:
            adduser(MCWEB_LDAP_DN, admin_password, cn, sn, uid, email, pw)
            print('%s has been added to the the mcweb LDAP db' % uid)
        except Exception as e:
            print('%s could not be added: %s' % (uid, e.__str__()))
        