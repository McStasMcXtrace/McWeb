from django.core.management.base import BaseCommand
from mcweb.settings import MCWEB_LDAP_DN
from signupper.ldaputils.ldaputils import chfield

class Command(BaseCommand):
    help = 'Changes a named field of a user account (use slapcat to figure out what the field name is).'

    def add_arguments(self, parser):
        parser.add_argument('admin_password', nargs=1, type=str, help='ldap admin password')
        parser.add_argument('uid', nargs=1, type=str, help='uid (username)')
        parser.add_argument('field', nargs=1, type=str, help='field name - uid, userpassword, mail, cn or sn')
        parser.add_argument('oldvalue', nargs=1, type=str, help='old/current value')
        parser.add_argument('newvalue', nargs=1, type=str, help='new value')

    def handle(self, *args, **options):
        admin_password = options['admin_password'][0]
        uid = options['uid'][0]
        field = options['field'][0]
        oldvalue = options['oldvalue'][0]
        newvalue = options['newvalue'][0]
        try:
            chfield(dn=MCWEB_LDAP_DN, admin_password=admin_password, uid=uid, value_name=field, current_value=oldvalue, new_value=newvalue)
            print('field "%s" of user "%s" was changed' % (field, uid))
        except Exception as e:
            print('field "%s" of user "%s" could not be changed: %s' % (field, uid, e.__str__()))
