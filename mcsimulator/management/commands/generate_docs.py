from django.core.management.base import BaseCommand

import glob, re

class Command(BaseCommand):
    
    def handle(self, *args, **options):
        grabbed = glob.glob('sim/*.instr')
        
        for instr in grabbed:
            src = file(instr).read()
            doc = re.search(r'^/\*\*+(.*)\%[Ee]', src, re.MULTILINE + re.DOTALL).group(1)
            out = instr + '.doc.txt'
            print('Writing file... %s' % out)
            file(out, 'w').write(doc)
        