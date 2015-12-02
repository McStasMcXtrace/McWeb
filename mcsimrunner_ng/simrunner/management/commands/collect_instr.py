'''
Collects all groups and instruments from disk and creates db objects. 
Already existing groups are unchanged, and already existing instruments 
are updated with current params info.
'''
from django.core.management.base import BaseCommand
import subprocess
import os
from simrunner.models import InstrGroup, Instrument

def get_group_instrs(basedir):
    ''' returns a dict of {group: [instr_files]}, assuming depth of one from basedir '''
    files_instr = []
    grp_instr = {}
    i = 0
    for (dirpath, dirname, files) in os.walk(basedir):
        if i == 0: 
            for d in dirname:
                grp_instr[d] = [] 
    
        for f in files:
            if os.path.splitext(f)[1] == '.instr':
                files_instr.append(dirpath + '/' + f)
        i += 1
    
    for f in files_instr:
        g = os.path.split(os.path.dirname(f))[1]
        grp_instr[g].append(os.path.splitext(os.path.basename(f))[0])
    
    return grp_instr

def get_instr_params(instr_file):
    ''' returns params [[name, value]] list of list, from instr_file (relative path) '''
    cmd = 'mcrun ' + instr_file + " --info"
    process = subprocess.Popen(cmd, 
                               stdout=subprocess.PIPE, 
                               stderr=subprocess.PIPE,
                               shell=True)
    (stdoutdata, stderrdata) = process.communicate()
    if process.returncode != 0:
        raise Exception('instrument compile error.')
    
    # get parameters from info
    params = []
    for l in stdoutdata.splitlines():
        if 'Param:' in l:
            s = l.split()[1]
            s = s.split('=')
            params.append(s)
            
    return params

class Command(BaseCommand):
    help = 'adds groups and contained instruments from disk to the db'
    
    def handle(self, *args, **options):
        grp_instr = get_group_instrs('sim/')
        
        for g in grp_instr:
            try:
                group = InstrGroup.objects.get(name=g)
                print "group %s" % g
            except InstrGroup.DoesNotExist:
                group = InstrGroup(name=g)
                print "group %s created" % g
                group.save()
                print "group %s saved" % g
            
            for i in grp_instr[g]:
                displayname = i
                name = g + "_" + i
                try: 
                    instr = Instrument.objects.get(name=name)
                except Instrument.DoesNotExist:    
                    instr = Instrument(group=group, name=name, displayname=displayname)
                    print "instrument %s created" % i
                
                # update instr params
                instr.params = get_instr_params(os.path.join('sim', g, i))
                instr.save()
                print "instrument %s saved" % i
                
        print 'collect_instr run done'
