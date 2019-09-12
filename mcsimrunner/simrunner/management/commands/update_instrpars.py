'''
Updates the pars of an existing instrumentwithout compiling it first.
'''
from django.core.management.base import BaseCommand
import subprocess
import os, errno
import shutil
import datetime
import re
from simrunner.models import InstrGroup, Instrument
from mcweb.settings import MCRUN, MXRUN
        
def get_group_instrs(basedir):
    ''' returns a dict of {group: [instr_files]}, assuming depth of one from basedir NOTE: "datafiles" is omited '''
    files_instr = []
    grp_instr = {}
    i = 0
    for (dirpath, dirname, files) in os.walk(basedir):
        if i == 0: 
            for d in dirname:
                if not d == 'datafiles':
                    grp_instr[d] = []
                else:
                    print('avoiding folder: datafiles')
    
        for f in files:
            if os.path.splitext(f)[1] == '.instr':
                files_instr.append(dirpath + '/' + f)
        i += 1
    
    for f in files_instr:
        g = os.path.split(os.path.dirname(f))[1]
        grp_instr[g].append(os.path.splitext(os.path.basename(f))[0])
    
    return grp_instr

def get_instr_params_and_set_affiliation(instr_grp, instr_displayname, instr_obj):
    ''' returns params [[name, value]] list of list, from instr_displayname (relative path) '''
    MCCODE = MCRUN
    instr_file = 'sim/' + instr_grp + '/' + instr_displayname + '.instr'
    
    # Check if this is McStas or McXtrace by a simple 
    for line in open(instr_file):
        if re.search('mcxtrace', line, re.IGNORECASE):
            MCCODE = MXRUN
            instr_obj.is_mcxtrace = True
            break
    
    cmd = MCCODE + ' --mpi=1 ' + instr_displayname + " --info"
    
    process = subprocess.Popen(cmd, 
                               stdout=subprocess.PIPE, 
                               stderr=subprocess.PIPE,
                               shell=True, 
                               cwd=os.path.join('sim', instr_grp))
    (stdoutdata, stderrdata) = process.communicate()
    if process.returncode != 0:
        raise Exception('instrument compile error: \n%s\n%s' % (stdoutdata, stderrdata))
    
    cmd2 = 'mcdoc.pl -t ./' + os.path.basename(instr_displayname)
    process2 = subprocess.Popen(cmd2, 
                               stdout=subprocess.PIPE, 
                               stderr=subprocess.PIPE,
                               shell=True, 
                               cwd=os.path.join('sim', instr_grp))
    (stdoutdata2, stderrdata2) = process2.communicate()
    
    if process.returncode != 0:
        raise Exception('instrument doc retrieval error.')
    # get parameters from info
    params = []
    for l in stdoutdata.splitlines():
        if 'Param:' in l:
            s = l.split()[1]
            s = s.split('=')
            for l2 in stdoutdata2.splitlines():
                if  '<' + s[0] in l2:
                    l2 = l2.split(':')[1]
                    try:
                        unit = l2.split('[')[1]
                        unit = unit.split(']')[0]
                        unit = '[' + unit + ']'
                    except:
                        unit = '(no unit)'
                    try:
                        info = l2.replace(unit, '')
                    except:
                        info = 'No info'
                    s.append(unit)
                    s.append(info)
            params.append(s)
    
    # check uniqueness of params:
    params_4real = []
    for p in params:
        if p not in params_4real:
            params_4real.append(p)
    
    return params_4real


class Command(BaseCommand):
    help = '''
        Updates parameters using an existing instrument name. Does not compile instruments, and updates the instrument
        db obj at most once pr. group using naiive name matching.
        '''

    def add_arguments(self, parser):
        parser.add_argument('instrname', nargs=1, type=str, help='instrument displayname and filename (before the dot)')

    def handle(self, *args, **options):
        displayname = options["instrname"][0]
        instrfilepaths = []

        basedir = "sim"
        for (_, dirnames, files) in os.walk(basedir):
            for d in dirnames:
                for (_, _, files) in os.walk(os.path.join(basedir, d)):
                    for f in files:
                        if f == displayname + ".instr":
                            ''' do stuff '''
                            instrfilepaths.append(os.path.join(basedir, d, f))

        try:
            objs = Instrument.objects.filter(displayname=displayname)

        except Instrument.DoesNotExist:    
            print "instrument %s not found" % displayname
            quit()

        update_lst = []
        nopdate_lst = []
        for f in instrfilepaths:
            #basename = os.path.basename(f)
            #displayname = os.path.splitext(basename)[0]

            for o in objs:
                if os.path.join("sim", o.group.name) == os.path.dirname(f):
                    oldparams = o.params
                    newparams = get_instr_params_and_set_affiliation(o.group.name, displayname, o)
                    if oldparams != newparams:
                        o.params = newparams
                        o.save()
                        update_lst.append(f)
                    else:
                        nopdate_lst.append(f)

        if len(update_lst)>0:
            print("updated db for:")
            for f in update_lst:
                print(f)
        if len(nopdate_lst)>0:
            print("did not update db for the following:")
            for f in nopdate_lst:
                print(f)
        if len(update_lst)==0 and len(nopdate_lst)==0:
            print("no matches were found")

