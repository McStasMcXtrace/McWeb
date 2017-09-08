from django.core.management.base import BaseCommand
from signupper.models import Signup

class Command(BaseCommand):
    help = 'Check signup entries for potential problems such as username/email doubles.'

    #def add_arguments(self, parser):
    #    parser.add_argument('--fix', action='store_true', dest='dry', default=False, help='Try to fix things.')

    def handle(self, *args, **options):
        
        signups = Signup.objects.all()
        
        # handle uid-non-singles
        hist = {}
        
        # collect all signups in a histogram
        for s in signups:
            uid = s.username
            if uid in hist: 
                hist[uid].append(s)
            else:
                hist[uid] = [s]
        
        # filter out all singles
        for uid in hist.keys():
            if len(hist[uid]) == 1:
                hist.pop(uid)
        
        # handle the non-singles
        state_prio = {0: 1, 1: 2, 2: 3, 3: 5, 4: 0, 5: 4}
        def prio_sort(signup, sp=state_prio):
            return float(sp[signup.state()]) + float(signup.created.strftime("%Y%m%d"))*0.00000001

        for uid in hist.keys():
            mimics = hist[uid]
            mimics = sorted(mimics, key=prio_sort, reverse=True)

            while len(mimics) > 1:
                mimics[-1].delete()
                mimics.pop(-1)
        
        
        '''
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

        print('')
        print('-- %d username doubles --' % len(p_uids))
        print('')
        for p in p_uids:
            print(p)
        print('')
        print('-- %d email doubles --' % len(p_emails))
        print('')
        for p in p_emails:
            print(p)
        '''
        
        
