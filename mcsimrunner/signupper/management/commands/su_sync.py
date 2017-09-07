from django.core.management.base import BaseCommand
from signupper.models import Signup
from signupper.ldaputils.ldaputils import synchronize as ldap_sync
from signupper.moodleutils.moodleutils import synchronize as moodle_sync

class Command(BaseCommand):
    help = 'Synchronize signup instances to ldap and moodle dbs.'

    def add_arguments(self, parser):
        parser.add_argument('--dry', action='store_true', dest='dry', default=False, help='Do not change the django db.')
    
    def handle(self, *args, **options):
        signups = Signup.objects.all()
        dry = True if options['dry'] else False
        
        ldap_sync(signups, dry)
        if dry:
            print("\nldap sync complete (dryrun)\n")
        else:
            print("\nldap sync complete\n")
        
        moodle_sync(signups, dry)
        if dry:
            print("moodle sync complete (dryrun)\n")
        else:
            print("moodle sync complete\n")
        
        print("All signups were synchronized with ldap and moodle.")
