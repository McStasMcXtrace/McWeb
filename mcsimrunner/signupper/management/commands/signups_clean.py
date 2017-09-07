from django.core.management.base import BaseCommand
from signupper.models import Signup

class Command(BaseCommand):
    help = 'Check signup entries for potential problems such as username/email doubles.'

    #def add_arguments(self, parser):
    #    parser.add_argument('--fix', action='store_true', dest='dry', default=False, help='Try to fix things.')

    def handle(self, *args, **options):
        
        signups = Signup.objects.all()
        
        uids = []
        emails = []
        p_uids = []
        p_emails = []
        
        for s in signups:
            uid = s.username 
            email = s.email
            
            if not uid in uids:
                uids.append(uid)
            else:
                p_uids.append('%s, %s' % (uid, email))
            
            if not email in emails:
                emails.append(email)
            else:
                p_emails.append('%s, %s' % (uid, email))
        
        print('-- username doubles --')
        for p in p_uids:
            print(p)
        print('-- email doubles --')
        for p in p_emails:
            print(p)
