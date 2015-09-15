from django.core.management.base import BaseCommand
from mcwww import settings

import re
import os

class Command(BaseCommand):
    
    def handle(self, *args, **options):
        
        instr_files = []
        for dirpath, _dirnames, filenames in os.walk(os.path.join(settings.BASE_DIR, 'sim')):
            for f in filenames:
                if re.search('.instr', f):
                    instr_files.append(os.path.join(dirpath, f))
        
        for instr in instr_files:
            src = file(instr).read()
            doc = re.search(r'^/\*\*+(.*)\%[Ee]', src, re.MULTILINE + re.DOTALL).group(1)
            out = instr + '.doc.txt'
            print('Writing file... %s' % out)
            file(out, 'w').write(doc)
        