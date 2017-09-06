from django.core.management.base import BaseCommand
from signupper.models import Signup
from signupper.ldaputils.ldaputils import synchronize as ldap_sync
from signupper.moodleutils.moodleutils import synchronize as moodle_sync
from mcweb.settings import MCWEB_LDAP_DN

class Command(BaseCommand):
    help = 'Synchronize signup instances to ldap and moodle dbs.'

    def add_arguments(self, parser):
        parser.add_argument('--dry', action='store_true', dest='dry', default=False, help='Do not change the django db.')
    
    def handle(self, *args, **options):
        signups = Signup.objects.all()
        
        ldap_sync(signups, dn=MCWEB_LDAP_DN)
        print("ldap sync complete")
        
        dry = True if options['dry'] else False
        moodle_sync(signups, dry)
        if dry:
            print("moodle sync complete (dryrun)")
        else:
            print("moodle sync complete")
        
        print("All signups were synchronized with ldap and moodle.")
