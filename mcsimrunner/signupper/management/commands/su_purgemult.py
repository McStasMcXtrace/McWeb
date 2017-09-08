from django.core.management.base import BaseCommand
from signupper.models import Signup

class Command(BaseCommand):
    help = 'Purges all doubles according to an aggressive procedure detailed in the code.'

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

