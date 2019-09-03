'''
When uploading a new version of an already-existing instrument, the simrun cache may need clearing.
'''
from django.core.management.base import BaseCommand
from simrunner.models import SimRun

class Command(BaseCommand):
    help = 'disables cache loading of simruns associated with an instrument'
    def add_arguments(self, parser):
        parser.add_argument('instrname', nargs=1, type=str, help='disable cache loading from simruns with this instrument name')

    def handle(self, *args, **options):
        instrname = options["instrname"][0]

        howmany = 0
        simruns = SimRun.objects.filter(instr_displayname=instrname)
        for s in simruns:
            if s.enable_cachefrom:
                s.enable_cachefrom = False
                s.save()
                howmany = howmany+1

        print("disabled caching from %d simruns" % howmany)

