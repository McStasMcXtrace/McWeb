from django.core.management.base import BaseCommand
from mcweb.settings import MCWEB_LDAP_DN
from signupper.ldaputils.ldaputils import listusers

class Command(BaseCommand):
    help = 'access the ldaputils.listusers function'

    def handle(self, *args, **options):
        users = listusers(MCWEB_LDAP_DN)
        for u in users:
            print(u)