'''
Lists db instrument objects.
'''
from django.core.management.base import BaseCommand
from simrunner.models import SimRun, Instrument

class Command(BaseCommand):
    help = 'list instrument objects currently in the django db'
    def handle(self, *args, **options):

        instrs = Instrument.objects.all()
        print("# displaying %d registered instrument objects..." % len(instrs))
        for i in instrs:
            print(i.displayname)

