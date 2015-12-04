'''
Simulation-running and -plotting thread/worker functions.
Based on SimRun model. Updates django db and is implemented as a django command.
Outputs comprehensive messages to stdout, which can be increased to include
mcrun, mcdisplay and mcplot stdout and stderr.
'''
from django.core.management.base import BaseCommand
from django.utils import timezone
from simrunner.models import SimRun
from mcweb.settings import STATIC_URL, SIM_DIR, DATA_DIRNAME, MCRUN_OUTPUT_DIRNAME
import subprocess
import os
import time
import shutil
import tarfile

class ExitException(Exception):
    ''' used to signal a runworker shutdown, rather than a simrun object fail-time and -string '''
    pass

def maketar(simrun):
    ''' makes the tar-file for download from the status page '''
    try: 
        with tarfile.open(os.path.join(simrun.data_folder, 'simrun.tar.gz'), "w:gz") as tar:
            tar.add(simrun.data_folder, arcname=os.path.basename(simrun.data_folder))
    except:
        raise Exception('tarfile fail')
    
def mcplot(simrun, print_mcplot_output=False):
    ''' generates plots from simrun output data '''
    outdir = os.path.join(simrun.data_folder, MCRUN_OUTPUT_DIRNAME)
    
    allfiles = [f for f in os.listdir(outdir) if os.path.isfile(os.path.join(outdir, f))]
    datfiles_nodir = [f for f in allfiles if os.path.splitext(f)[1] == '.dat']
    datfiles = map(lambda f: os.path.join(outdir, f), datfiles_nodir)
    
    plot_files = []
    plot_files_log = []
    plot_dir = os.path.join(STATIC_URL.lstrip('/'), DATA_DIRNAME, os.path.basename(simrun.data_folder), MCRUN_OUTPUT_DIRNAME)
    
    for f in datfiles: 
        cmd = 'mcplot-gnuplot-py -s %s' % f
        process = subprocess.Popen(cmd,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE,
                                   shell=True)
        (stdoutdata, stderrdata) = process.communicate()
        
        if print_mcplot_output:
            print(stdoutdata)
            if (stderrdata is not None) and (stderrdata != ''):
                raise Exception('mcplot error: %s' % stderrdata)
        
        p = os.path.basename(f)
        p = os.path.splitext(p)[0] + '.png'
        p = os.path.join(MCRUN_OUTPUT_DIRNAME, p)
        
        print('plot: %s' % p)
        plot_files.append(p)
    
    # NOTE: the following only works with mcplot-gnuplot-py
    for f in datfiles: 
        cmd = 'mcplot-gnuplot-py -l -s %s' % f
        process = subprocess.Popen(cmd,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE,
                                   shell=True)
        (stdoutdata, stderrdata) = process.communicate()
        
        if print_mcplot_output:
            print(stdoutdata)
            if (stderrdata is not None) and (stderrdata != ''):
                raise Exception('mcplot error: %s' % stderrdata)
        
        p = os.path.basename(f)
        p = os.path.splitext(p)[0] + '_log.png'
        p = os.path.join(MCRUN_OUTPUT_DIRNAME, p)
        
        print('plot: %s' % p)
        plot_files_log.append(p)
    
    simrun.plot_files = plot_files
    simrun.plot_files_log = plot_files_log
    simrun.save()

def clear_c_out_files(simrun):
    ''' removes .c and .out files from simrun data dir (NOTE: data dir should exists else exception). '''
    try: 
        dataf = simrun.data_folder
        dname = simrun.instr_displayname
        
        f_c = dataf + '/' + dname + '.c'
        if os.path.exists(f_c) or os.path.exists(f_c):
            os.remove(f_c)
        f_out = dataf + '/' + dname + '.out'
        if os.path.exists(f_out) or os.path.exists(f_out):
            os.remove(f_out)
        
    except Exception as e:
        raise Exception('clear_c_out_files: error clearing directory (%s)' % e.__str__())

def mcdisplay(simrun, print_mcdisplay_output=False):
    ''' uses mcdisplay to generate layout.png + VRML file and moves these files to simrun.data_folder '''
    # assemble command (WARNING: mcdisplay dont like abs paths, therefore we assume project root and assemble relpath)
    static_dirname = STATIC_URL.lstrip('/')
    instr_relpath = '%s/%s.instr' % (os.path.join(static_dirname, DATA_DIRNAME, os.path.basename(simrun.data_folder)), simrun.instr_displayname)
    
    cmd = 'mcdisplay -png --multi %s -n1 ' % (instr_relpath)
    vrmlcmd = 'mcdisplay --format=VRML %s -n1 ' % (instr_relpath)
    for p in simrun.params:
        cmd = cmd + ' %s=%s' % (p[0], p[1])
        vrmlcmd = vrmlcmd + ' %s=%s' % (p[0], p[1])
        
    # start mcdisplay process, wait
    process = subprocess.Popen(cmd,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               shell=True)
    (stdoutdata, stderrdata) = process.communicate()
    process2 = subprocess.Popen(vrmlcmd,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               shell=True)
    (stdoutdata2, stderrdata2) = process2.communicate()
    
    if print_mcdisplay_output:
        print(stdoutdata)
        if (stderrdata is not None) and (stderrdata != ''):
            print(stderrdata)
    if print_mcdisplay_output:
        print(stdoutdata2)
        if (stderrdata2 is not None) and (stderrdata2 != ''):
            print(stderrdata2)    
    
    oldfilename = '%s.out.png' % os.path.join(simrun.data_folder, simrun.instr_displayname)
    newfilename = os.path.join(simrun.data_folder, 'layout.png')
    oldwrlfilename = 'mcdisplay_commands.wrl'
    newwrlfilename = os.path.join(simrun.data_folder, 'layout.wrl')
    if os.path.exists(simrun.data_folder):
        os.rename(oldfilename, newfilename)
        os.rename(oldwrlfilename, newwrlfilename)
    else:
        raise Exception('Data folder must exist before running this function (runworker.mcdisplay).')
        
    print 'layout: %s' % newfilename
    print 'layout: %s' % newwrlfilename

def mcrun(simrun, print_mcrun_output=False):
    ''' runs the simulation associated with simrun '''
    # sanity reset
    clear_c_out_files(simrun)
    
    # assemble the run command (NOTE: if we wanted e.g. "mpi=4"
    instr = os.path.join(simrun.data_folder, simrun.instr_displayname)
    runstr = 'mcrun ' + instr + ' -d ' + os.path.join(simrun.data_folder, MCRUN_OUTPUT_DIRNAME)
    runstr = runstr + ' -n ' + str(simrun.neutrons)
    runstr = runstr + ' -N ' + str(simrun.scanpoints)
    if simrun.seed > 0:
        runstr = runstr + ' -s ' + str(simrun.seed)
    for p in simrun.params:
        runstr = runstr + ' ' + p[0] + '=' + p[1]
    
    print('simrun (%s)...' % runstr)
    
    # TODO: , cwd=os.path.join('sim', instr_grp)
    process = subprocess.Popen(runstr,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               shell=True)
    
    # TODO: implement a timeout (max simulation time)
    (stdout, stderr) = process.communicate()
    
    o = open('%s/stdout.txt' % simrun.data_folder, 'w')
    o.write(stdout)
    o.close()
    e = open('%s/stderr.txt' % simrun.data_folder, 'w')
    e.write(stderr)
    e.close()
    
    if process.returncode != 0:
        raise Exception('Instrument run failure (see stdout and stderr in data dir).')
    
    print('data: %s' % simrun.data_folder)

def init_processing(simrun):
    ''' creates data folder, copies instr files and updates simrun object '''
    try: 
        simrun.data_folder = os.path.join(os.path.join(STATIC_URL.lstrip('/'), DATA_DIRNAME), simrun.__str__())
        os.mkdir(simrun.data_folder)
        simrun.save()
        
        # copy instrument from sim folder to simrun data folder 
        instr_source = '%s/%s/%s.instr' % (SIM_DIR, simrun.group_name, simrun.instr_displayname)
        instr = '%s/%s.instr' % (simrun.data_folder, simrun.instr_displayname)
        shutil.copyfile(instr_source, instr)
        
    except Exception as e: 
        raise Exception('init_processing failed: %s' % e.__str__())

def check_age(simrun, max_mins):
    ''' checks simrun age: raises an exception if age is greater than max_mins. (Does not alter object simrun.) '''
    age = simrun.started - simrun.created
    age_mins = age.seconds / 60
    if age_mins > max_mins:
        raise Exception('Age of object has timed out for %s running %s at time %s).' % 
            (simrun.owner_username,simrun.instr_displayname ,simrun.created.strftime("%H:%M:%S_%Y-%m-%d")))

def work():
    ''' gets all non-started SimRun objects, updates statuses and calls sim, layout display and plot functions '''
    simrun_set = SimRun.objects.filter(started=None)
    
    for simrun in simrun_set:
        # exceptions raised during the processing block are written to the simrun object as fail, but do not break the processing loop
        try:
            # mark object as processing initiated
            print('processing simrun for %s...' % simrun.instr_displayname)
            simrun.started = timezone.now()
            simrun.save()
            
            # check simrun object age
            check_age(simrun, max_mins=30)
            
            # init processing
            init_processing(simrun)
            
            # process
            mcrun(simrun)
            mcdisplay(simrun)
            mcplot(simrun)
            
            # post-processing
            maketar(simrun)
            
            # finish
            simrun.complete = timezone.now()
            simrun.save()
            
            print('done (%s secs).' % (simrun.complete - simrun.started).seconds)
            
        except Exception as e:
            if e is ExitException:
                exit()

            simrun.failed = timezone.now()
            simrun.fail_str = e.__str__()
            simrun.save()
            print('simrun fail: %s') % e.__str__()
    
    if len(simrun_set) > 0:
        print("idle...")

class Command(BaseCommand):
    ''' django simrun processing command "runworker" '''
    help = 'generates mcstas/mxtrace simulation output and updates db'

    def add_arguments(self, parser):
        ''' adds the debug run option (run only once) '''
        parser.add_argument('--debug', action='store_true', help="runs work() only once")
        
    def handle(self, *args, **options):
        ''' implements main execution loop and debug run '''
        
        # ensure data output dir exists: 
        try:
            data_basedir = os.path.join(STATIC_URL.lstrip('/'), DATA_DIRNAME)
            if not os.path.exists(data_basedir):
                os.mkdir(data_basedir)
        except:
            raise ExitException('Could not find or create base data folder, exiting (%s).' % data_basedir)            
        
        # graceful exiting
        try:
            # debug run
            if options['debug']:
                work()
                exit()
            
            print("looking for simruns...")
            
            # main runworker execution loop
            while True:
                work()
                time.sleep(1)
        
        # ctr-c exits
        except KeyboardInterrupt:
            print ""
            print "shutdown requested, exiting..."
