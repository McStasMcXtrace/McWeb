from django.core.management.base import BaseCommand
from signupper.models import Signup
from signupper.ldaputils.ldaputils import synchronize as ldap_sync
from signupper.moodleutils.moodleutils import synchronize as moodle_sync
from mcweb.settings import MCWEB_LDAP_DN

class Command(BaseCommand):
    help = 'Synchronize signup instances to ldap and moodle dbs.'

    def handle(self, *args, **options):
        signups = Signup.objects.all()
        
        ldap_sync(signups, dn=MCWEB_LDAP_DN)
        print("ldap sync complete")
        
        moodle_sync(signups)
        print("moodle sync complete")
        
        print("All signups were synchronized with ldap and moodle.")
