from django.core.management.base import BaseCommand
from mcweb.settings import MCWEB_LDAP_DN
from signupper.ldaputils.ldaputils import _chfield

class Command(BaseCommand):
    help = 'Changes the sn/last name of a user account.'

    def add_arguments(self, parser):
        parser.add_argument('admin_password', nargs=1, type=str, help='ldap admin password')
        parser.add_argument('uid', nargs=1, type=str, help='uid (username) of user to be added')
        parser.add_argument('field', nargs=1, type=str, help='field name ')
        parser.add_argument('oldvalue', nargs=1, type=str, help='old/current value')
        parser.add_argument('newvalue', nargs=1, type=str, help='new value')

    def handle(self, *args, **options):
        ''' Implements the django ldap_chfield command '''
        admin_password = options['admin_password'][0]
        uid = options['uid'][0]
        field = options['field'][0]
        oldvalue = options['oldvalue'][0]
        newvalue = options['newvalue'][0]
        try:
            _chfield(dn=MCWEB_LDAP_DN, admin_password=admin_password, uid=uid, value_name=field, current_value=oldvalue, new_value=newvalue)
            print('%s has been added to the the mcweb LDAP db' % uid)
        except Exception as e:
            print('%s could not be added: %s' % (uid, e.__str__()))
