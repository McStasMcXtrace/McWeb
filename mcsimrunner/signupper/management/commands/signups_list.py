from django.core.management.base import BaseCommand
from signupper.models import Signup

class Command(BaseCommand):
    help = 'List signups based on state (see signupper.models docs for details).'

    def add_arguments(self, parser):
        parser.add_argument('state', nargs='?', type=str, help='new value')
        parser.add_argument('--verbose', action='store_true', dest='verbose', default=False, help='Display entries.')

    def handle(self, *args, **options):
        state = options['state']
        states = [0, 1, 2, 3, 4, 5] if not state else [int(state[0])]
        for state in states:
            signups = Signup.objects.all()
            subset = [s for s in signups if s.state() == state]
            print("-- %s signups in state %s --" % (len(subset), state))
            if options['verbose']:
                for s in subset:
                    print("%s" % s.username)
        
