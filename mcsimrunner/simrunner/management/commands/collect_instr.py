'''
Collects all groups and instruments from disk and creates db objects. 
Already existing groups are unchanged, and already existing instruments 
are updated with current params info.
'''
from django.core.management.base import BaseCommand
import subprocess
import os, errno
import shutil
from simrunner.models import InstrGroup, Instrument
from mcweb.settings import MCRUN

def mkdir_p(path):
    ''' create directory ala Unix mkdir -p '''
    try:
                os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
                        pass
        else: raise

def make_html_docs(group):
    ''' run mcdoc in instrument group folder, move html files to static/doc/..., create links back to the instr files '''
    cmd = 'mcdoc -t .' 
    process = subprocess.Popen(cmd, 
                               stdout=subprocess.PIPE, 
                               stderr=subprocess.PIPE,
                               shell=True, 
                               cwd='sim/' + group)
    (stdoutdata, stderrdata) = process.communicate()
    if process.returncode != 0:
        raise Exception('instrument doc generation error.')

    cmd = 'mv sim/' + group + '/*.html static/doc/' + group + '/'
    process = subprocess.Popen(cmd, 
                               stdout=subprocess.PIPE, 
                               stderr=subprocess.PIPE,
                               shell=True)
    (stdoutdata, stderrdata) = process.communicate()
    
    cmd = 'ln -s $PWD/sim/' + group + '/*.instr static/doc/' + group + '/'
    process = subprocess.Popen(cmd,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               shell=True)
    (stdoutdata, stderrdata) = process.communicate()
        
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

def get_instr_params(instr_grp, instr_file):
    ''' returns params [[name, value]] list of list, from instr_file (relative path) '''
    
    cmd = MCRUN + ' --mpi ' + instr_file + " --info"
    process = subprocess.Popen(cmd, 
                               stdout=subprocess.PIPE, 
                               stderr=subprocess.PIPE,
                               shell=True, 
                               cwd=os.path.join('sim', instr_grp))
    (stdoutdata, stderrdata) = process.communicate()
    if process.returncode != 0:
        raise Exception('instrument compile error: \n%s\n%s' % (stdoutdata, stderrdata))
    
    cmd2 = 'mcdoc -t ./' + os.path.basename(instr_file)
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
    help = 'adds groups and contained instruments from disk to the db'
    
    def handle(self, *args, **options):
        print 'collecting instruments one depth in sim/...'
        
        grp_instr = get_group_instrs('sim/')
        for g in grp_instr:
            try:
                group = InstrGroup.objects.get(name=g)
                print "group %s exists in db" % g
            except InstrGroup.DoesNotExist:
                group = InstrGroup(name=g)
                group.save()
                print "group %s created" % g
        
            mkdir_p( "static/doc/%s" % g )
            print "doc folder static/doc/%s touched" % g
            
            try:
                make_html_docs(g)
            except:
                raise Exception('make_html_docs failed')
                
            for i in grp_instr[g]:
                displayname = i
                name = g + "_" + i
                try: 
                    instr = Instrument.objects.get(name=name)
                    print "instrument %s exists in db" % i
                except Instrument.DoesNotExist:    
                    instr = Instrument(group=group, name=name, displayname=displayname)
                    print "instrument %s created" % i
                    
                if os.path.isfile('sim/' + g + '/' + i + '.png'):
                    instr.image = "/static/doc/" + g + "/" + i + ".png"
                    shutil.copyfile('sim/' + g + '/' + i + '.png','static/doc/' + g + '/' + i + '.png')
                    print "Adding image for instrument %s" % i
                else:
                    instr.image = '/static/noimage.png'

                # update instr params
                instr.params = get_instr_params(g, i)
                instr.save()
                print "instrument %s params updated" % i
                
        print 'collect_instr done.'
