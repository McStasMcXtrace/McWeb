'''
Simulation-running and -plotting thread/worker functions.
Based on SimRun model. Updates django db and is implemented as a django command.
Outputs comprehensive messages to stdout, which can be increased to include
mcrun, mcdisplay and mcplot stdout and stderr.
'''
from django.core.management.base import BaseCommand
from django.utils import timezone
from simrunner.models import SimRun
import subprocess
import os
import time

class ExitException(Exception):
    ''' used to signal a runworker shutdown, rather than a simrun fail '''
    pass

def mcplot(simrun, print_mcplot_output=False):
    ''' generates plots from simrun output data '''
    allfiles = [f for f in os.listdir(simrun.data_folder) if os.path.isfile(os.path.join(simrun.data_folder, f))]
    datfiles_nodir = [f for f in allfiles if os.path.splitext(f)[1] == '.dat']
    datfiles = map(lambda f: os.path.join(simrun.data_folder, f), datfiles_nodir)
    plot_files = []
    
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
        
        p = os.path.splitext(f)[0] + '.png'
        print('plot: %s' % p)
    
        plot_files.append(p)
    
    simrun.plot_files = plot_files
    simrun.save()

def mcdisplay(simrun, print_mcdisplay_output=False):
    ''' uses mcdisplay to generate layout.png and moves this file to simrun.data_folder '''
    cmd = 'mcdisplay -png --multi %s -n1 ' % (simrun.instr_filepath)
    for p in simrun.params:
        cmd = cmd + ' %s=%s' % (p[0], p[1])
    
    process = subprocess.Popen(cmd,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               shell=True)
    (stdoutdata, stderrdata) = process.communicate()
    if print_mcdisplay_output:
        print(stdoutdata)
        if (stderrdata is not None) and (stderrdata != ''):
            print(stderrdata)
    
    oldfilename = '%s.out.png' % os.path.splitext(simrun.instr_filepath)[0]
    newfilename = os.path.join(simrun.data_folder, 'layout.png')
    if os.path.exists(simrun.data_folder):
        os.rename(oldfilename, newfilename)
    else:
        raise Exception('Data folder must exist before running this function (runworker.mcdisplay).')
        
    print 'layout: %s' % newfilename

def mcrun(simrun, print_mcrun_output=False):
    ''' runs the simulation associated with simrun '''
    # assemble the run command 
    # NOTE: if we wanted e.g. "mpi=4", then that goes before instr_filepath
    runstr = 'mcrun ' + simrun.instr_filepath + ' -d ' + simrun.data_folder
    runstr = runstr + ' -n ' + str(simrun.neutrons)
    runstr = runstr + ' -N ' + str(simrun.scanpoints)
    if simrun.seed > 0:
        runstr = runstr + ' -s ' + str(simrun.seed)
    for p in simrun.params:
        runstr = runstr + ' ' + p[0] + '=' + p[1]
    
    print('simrun (%s)...' % runstr)
    
    process = subprocess.Popen(runstr,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               shell=True)
    
    # TODO: implement a timeout (max simulation time)
    (stdoutdata, stderrdata) = process.communicate()
    if print_mcrun_output:
        print(stdoutdata)
        if (stderrdata is not None) and (stderrdata != ''):
            print(stderrdata)
    
    if process.returncode != 0:
        raise Exception('Instrument compile error.')
    
    print('data: %s' % simrun.data_folder)

def create_instr_filepath(instr_basedir, group_name, instr_displayname):
    ''' constructs and returns full instrument filepath (system is tested for relative paths) '''
    return '%s/%s/%s.instr' % (instr_basedir, group_name, instr_displayname)

def get_data_folderpath(subfolder, basefolder):
    ''' checks if basefolder exists, returns data folder (does not create it) '''
    if not os.path.exists(basefolder):
        raise ExitException('Base output folder %s does not exist, exiting.' % basefolder)
    
    return os.path.join(basefolder, subfolder) 

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
            simrun.data_folder = get_data_folderpath(simrun.__str__(), 'out')
            simrun.instr_filepath = create_instr_filepath('sim', simrun.group_name, simrun.instr_displayname)
            simrun.save()
            
            # process
            mcrun(simrun)
            mcdisplay(simrun)
            mcplot(simrun)
            
            # finish
            simrun.complete = timezone.now()
            simrun.save()
            
            print('done (%s secs).' % (simrun.complete - simrun.started).seconds)
            
        except Exception as e:
            if e is ExitException:
                exit()

            simrun.failed = timezone.now()
            simrun.fail_str = e.message
            simrun.save()
            print('simrun fail: %s') % e.message
    
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

        # debug run
        if options['debug']:
            work()
            exit()
        
        print("looking for simruns...")
        
        # main runworker execution loop
        while True:
            work()
            time.sleep(1)
