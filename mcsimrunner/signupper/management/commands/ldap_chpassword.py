from django.core.management.base import BaseCommand
from mcweb.settings import MCWEB_LDAP_DN
from signupper.ldaputils.ldaputils import chpassword

class Command(BaseCommand):
    help = 'Changes the password of a user account.'

    def add_arguments(self, parser):
        parser.add_argument('admin_password', nargs=1, type=str, help='ldap admin password')
        parser.add_argument('uid', nargs=1, type=str, help='uid (username)')
        parser.add_argument('old', nargs=1, type=str, help='old/current password value')
        parser.add_argument('new', nargs=1, type=str, help='new password value')

    def handle(self, *args, **options):
        admin_password = options['admin_password'][0]
        uid = options['uid'][0]
        oldvalue = options['oldvalue'][0]
        newvalue = options['newvalue'][0]
        try:
            chpassword(dn=MCWEB_LDAP_DN, admin_password=admin_password, uid=uid, value_name='userpassword', current_value=oldvalue, new_value=newvalue)
            print('user "%s" password was changed' % uid)
        except Exception as e:
            print('password of user "%s" could not be changed: %s' % (uid, e.__str__()))
