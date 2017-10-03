'''
Simulation-running and -plotting thread/worker functions.
Based on SimRun model. Updates django db and is implemented as a django command.
Outputs comprehensive messages to stdout, which can be increased to include
mcrun, mcdisplay and mcplot stdout and stderr.
'''
import subprocess
import os
import time
import tarfile
import threading
import logging
import re

from django.core.management.base import BaseCommand
from django.utils import timezone
from simrunner.models import SimRun
from mcweb.settings import STATIC_URL, SIM_DIR, DATA_DIRNAME, MCRUN_OUTPUT_DIRNAME, MCPLOT_CMD, MCPLOT_LOGCMD, MPI_PR_WORKER, MCRUN
import mcweb.settings as settings

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
    
def plot_file(f, log=False):
    f_dirname=os.path.dirname(f)
    f_filename=os.path.basename(f)
    cmd = '%s %s' % (MCPLOT_CMD,f_filename)
    if log:
        cmd = '%s %s' % (MCPLOT_LOGCMD,f_filename)
    process = subprocess.Popen(cmd,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               shell=True, cwd=f_dirname)
    (stdoutdata, stderrdata) = process.communicate()
    if os.path.isfile( f + '.png'):
        if log:
            os.rename(f + '.png',os.path.splitext(os.path.splitext(f)[0])[0] + '_log.png')
        else:    
            os.rename(f + '.png',os.path.splitext(os.path.splitext(f)[0])[0] + '.png')
    return (stdoutdata, stderrdata)
    
def rename_mcstas_to_mccode(simrun):
    ''' run before mcplot to avoid issues with old versions of mcstas '''
    for token in ['.sim', '.dat']:
        wrong = os.path.join(simrun.data_folder, 'mcstas%s' % token)
        right = os.path.join(simrun.data_folder, 'mccode%s' % token)
        if os.path.exists(wrong):
            os.rename(wrong, right)

def get_monitor_files(mccode_sim):
    ''' implements "data files can have any name" '''
    monitor_files = filter(lambda line: (line.strip()).startswith('filename:'), open(mccode_sim).readlines())
    monitor_files = map(lambda f: f.rstrip('\n').split(':')[1].strip(), monitor_files)
    return monitor_files

def mcplot(simrun):
    ''' generates plots from simrun output data '''
    rename_mcstas_to_mccode(simrun)
    
    try:
        if simrun.scanpoints > 1:
            # init
            plot_files = []
            plot_files_log = []
            data_files = []
            
            # plot and store mccode.dat, which must exist
            f = os.path.join(simrun.data_folder, MCRUN_OUTPUT_DIRNAME, 'mccode.dat')
            
            plot_file(f)
            p = os.path.basename(f)
            p = os.path.splitext(p)[0] + '.png'
            p = os.path.join(MCRUN_OUTPUT_DIRNAME, p)
            plot_files.append(p)
            
            plot_file(f, log=True)
            p_log = os.path.basename(f)
            p_log = os.path.splitext(p_log)[0] + '.png'
            p_log = os.path.join(MCRUN_OUTPUT_DIRNAME, p_log)
            plot_files_log.append(p_log)
            
            d = os.path.basename(f)
            d = os.path.join(MCRUN_OUTPUT_DIRNAME, d)
            data_files.append(d)
            
            logging.info('plot_linlog: %s' % p)
            
            for i in range(simrun.scanpoints):
                if i > 0:
                    logging.info('plot_linlog (scanpoint index %d)...' % i)
                
                outdir = os.path.join(simrun.data_folder, MCRUN_OUTPUT_DIRNAME, str(i))
                
                datfiles_nodir = get_monitor_files(os.path.join(outdir, 'mccode.sim'))
                datfiles = map(lambda f: os.path.join(outdir, f), datfiles_nodir)
                
                for f in datfiles:
                    plot_file(f)
                    plot_file(f, log=True)
                    
                    p = os.path.basename(f)
                    p = os.path.splitext(p)[0] + '.png'
                    p = os.path.join(MCRUN_OUTPUT_DIRNAME, str(i), p)
                    
                    p_log = os.path.basename(f)
                    p_log = os.path.splitext(p_log)[0] + '.png'
                    p_log = os.path.join(MCRUN_OUTPUT_DIRNAME, str(i), p_log)
                    
                    d = os.path.basename(f)
                    d = os.path.join(MCRUN_OUTPUT_DIRNAME, str(i), d)
                    
                    if i == 0:
                        logging.info('plot_linlog: %s' % p)
                        plot_files.append(p)
                        plot_files_log.append(p_log)
                        data_files.append(d)
                
        else:
            outdir = os.path.join(simrun.data_folder, MCRUN_OUTPUT_DIRNAME)
            
            datfiles_nodir = get_monitor_files(os.path.join(outdir, 'mccode.sim'))
            datfiles = map(lambda f: os.path.join(outdir, f), datfiles_nodir)
            
            data_files = []
            plot_files = []
            plot_files_log = []
        
            for f in datfiles: 
                plot_file(f)
                
                p = os.path.basename(f)
                p = os.path.splitext(p)[0] + '.png'
                p = os.path.join(MCRUN_OUTPUT_DIRNAME, p)
                
                logging.info('plot: %s' % p)
                plot_files.append(p)
            
            # NOTE: the following only works with mcplot-gnuplot-py
            for f in datfiles: 
                plot_file(f, log=True)
                
                l = os.path.basename(f)
                l = os.path.splitext(l)[0] + '_log.png'
                l = os.path.join(MCRUN_OUTPUT_DIRNAME, l)
                
                logging.info('plot: %s' % l)
                plot_files_log.append(l)
            
            for f in datfiles:
                d = os.path.basename(f)
                d = os.path.join(MCRUN_OUTPUT_DIRNAME, d)
                
                data_files.append(d)
            
        simrun.data_files = data_files
        simrun.plot_files = plot_files
        simrun.plot_files_log = plot_files_log
        simrun.save()

    except Exception as e:
        raise Exception('mcplot fail: %s' % e.__str__())

def mcdisplay_webgl(simrun, pout=False):
    ''' apply mcdisplay-webgl output to subfolder 'mcdisplay', renaming index.html to mcdisplay.html '''
    join = os.path.join
    
    dirname = 'mcdisplay'
    instr = '%s.instr' % simrun.instr_displayname

    params_str_lst = []
    for p in simrun.params:
        # scan sweep special case - use last scanpoint
        m = re.search(',\s*([0-9\.]+)', p[1])
        if m:
            p[1] = m.group(1)
        params_str_lst.append('%s=%s' % (p[0], p[1]))
    params_str = ' '.join(params_str_lst)
    cmd = '%s %s %s --nobrowse --dir=%s' % (settings.MCDISPLAY_WEBGL, instr, params_str, dirname)
    
    # TODO: inplement --inspect, --first, --last
    
    # run mcdisplay
    logging.info('display: %s' % cmd)
    process = subprocess.Popen(cmd,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               shell=True,
                               cwd = '/srv/mcweb/McWeb/mcsimrunner/' + simrun.data_folder)
    (stdoutdata, stderrdata) = process.communicate()
    if pout:
        print(stdoutdata)
        if (stderrdata is not None) and (stderrdata != ''):
            print(stderrdata)
    
    # copy files
    #logging.info('mcdisplay: renaming index.html')
    #os.rename(join(simrun.data_folder, dirname, 'index.html'), join(simrun.data_folder, dirname, 'mcdisplay.html'))

def mcdisplay(simrun, print_mcdisplay_output=False):
    ''' uses mcdisplay to generate layout.png + VRML file and moves these files to simrun.data_folder '''
    try:
        instr = '%s.instr' % simrun.instr_displayname
        
        cmd = '%s -png --multi %s -n1 ' % (settings.MCDISPLAY, instr)
        vrmlcmd = '%s --format=VRML %s -n1 ' % (settings.MCDISPLAY, instr)
        
        for p in simrun.params:
            s = str(p[1])
            # support for scan sweeps; if a param contains comma, get str before (mcdisplay dont like comma)
            # (NOTE: perhaps better to make a layout.png file for every scan point)
            if ',' in s:
                s = s.split(',')[0]  
            cmd = cmd + ' %s=%s' % (p[0], s)
            vrmlcmd = vrmlcmd + ' %s=%s' % (p[0], s)
        
        # start mcdisplay process, wait
        process = subprocess.Popen(cmd,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE,
                                   shell=True,
                                   cwd = simrun.data_folder)
        (stdoutdata, stderrdata) = process.communicate()
        process2 = subprocess.Popen(vrmlcmd,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE,
                                   shell=True, 
                                   cwd = simrun.data_folder)
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
        oldwrlfilename = os.path.join(simrun.data_folder,'mcdisplay_commands.wrl')
        newwrlfilename = os.path.join(simrun.data_folder, 'layout.wrl')
        
        os.rename(oldfilename, newfilename)
        os.rename(oldwrlfilename, newwrlfilename)
        
        logging.info('layout: %s' % newfilename)
        logging.info('layout: %s' % newwrlfilename)
        
    except Exception as e:
        logging.error('mcdisplay fail: %s \nwith stderr:      %s \n     stderr_wrml: %s' % (e.__str__(), stderrdata, stderrdata2))
    
def mcrun(simrun, print_mcrun_output=False):
    ''' runs the simulation associated with simrun '''
    # assemble the run command 
    runstr = MCRUN + ' --mpi=' + str(MPI_PR_WORKER) + " " + simrun.instr_displayname + '.instr -d ' + MCRUN_OUTPUT_DIRNAME
    runstr = runstr + ' -n ' + str(simrun.neutrons)
    if simrun.scanpoints > 1:
        runstr = runstr + ' -N ' + str(simrun.scanpoints)
    if simrun.seed > 0:
        runstr = runstr + ' -s ' + str(simrun.seed)
    for p in simrun.params:
        runstr = runstr + ' ' + p[0] + '=' + p[1]
    
    # create empty stdout.txt and stderr.txt files
    f = open('%s/stdout.txt' % simrun.data_folder, 'w')
    f.write("no stdout data for: %s" % runstr)
    f.close()
    f = open('%s/stderr.txt' % simrun.data_folder, 'w')
    f.write("no stderr data for: %s" % runstr)
    f.close()
    
    logging.info('running: %s...' % runstr)
    process = subprocess.Popen(runstr,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               shell=True,
                               cwd=simrun.data_folder)
    # TODO: implement a timeout (max simulation time)
    (stdout, stderr) = process.communicate()
    
    o = open('%s/stdout.txt' % simrun.data_folder, 'w')
    o.write(stdout)
    o.close()
    e = open('%s/stderr.txt' % simrun.data_folder, 'w')
    e.write(stderr)
    e.close()
    
    if process.returncode != 0:
        raise Exception('Instrument run failure - see %s.' % simrun.data_folder )
    
    logging.info('data: %s' % simrun.data_folder)

def init_processing(simrun):
    ''' creates data folder, copies instr files and updates simrun object '''
    try: 
        simrun.data_folder = os.path.join(os.path.join(STATIC_URL.lstrip('/'), DATA_DIRNAME), simrun.__str__())
        os.mkdir(simrun.data_folder)
        simrun.save()
        
        # copy instrument from sim folder to simrun data folder 
        instr_source = '%s/%s/%s.instr' % (SIM_DIR, simrun.group_name, simrun.instr_displayname)
        instr = '%s/%s.instr' % (simrun.data_folder, simrun.instr_displayname)
        p = subprocess.Popen(['cp','-p',instr_source, instr])
        p.wait()

        # symlink the .c and the .out files
        src_c = '%s/%s/%s.c' % (SIM_DIR, simrun.group_name, simrun.instr_displayname)
        src_out = '%s/%s/%s.out' % (SIM_DIR, simrun.group_name, simrun.instr_displayname)
        ln_c = '%s/%s.c' % (simrun.data_folder, simrun.instr_displayname)
        ln_out = '%s/%s.out' % (simrun.data_folder, simrun.instr_displayname)
        p = subprocess.Popen(['cp','-p',src_c, ln_c])
        p.wait()
        p = subprocess.Popen(['cp','-p',src_out, ln_out])
        p.wait()

        # symlink the contents of sim/datafiles/
        
        allfiles = [f for f in os.listdir('sim/datafiles/') if os.path.isfile(os.path.join('sim/datafiles/', f))]
        if '.gitignore' in allfiles:
            allfiles.remove('.gitignore')
        for f in allfiles:
            src = os.path.join('..', '..', '..', 'sim', 'datafiles', f)
            ln = '%s/%s' % (simrun.data_folder, f)
            os.symlink(src, ln)
        
    except Exception as e:
        raise Exception('init_processing: %s (%s)' % (type(e).__name__, e.__str__()))

def check_age(simrun, max_mins):
    ''' checks simrun age: raises an exception if age is greater than max_mins. (Does not alter object simrun.) '''
    age = simrun.started - simrun.created
    age_mins = age.seconds / 60
    if age_mins > max_mins:
        raise Exception('Age of object has timed out for %s running %s at time %s).' % 
            (simrun.owner_username,simrun.instr_displayname ,simrun.created.strftime("%H:%M:%S_%Y-%m-%d")))

def get_and_start_new_simrun():
    ''' gets an unstarted simrun from the db, sets its status to "running" and return it. Otherwise it returns None '''
    simrun = None
    simrun_set = SimRun.objects.filter(started=None)
    if len(simrun_set) > 0:
        simrun = simrun_set[0]

        simrun.started = timezone.now()
        simrun.save()
        
    return simrun

def threadwork(simrun):
    ''' thread method for simulation and plotting '''
    try:
        # check simrun object age
        check_age(simrun, max_mins=3600)
        
        # init processing
        init_processing(simrun)
        
        # process
        mcrun(simrun)
        mcdisplay_webgl(simrun)
        mcdisplay(simrun)
        mcplot(simrun)
        
        # post-processing
        maketar(simrun)
        
        # finish
        simrun.complete = timezone.now()
        simrun.save()
        
        logging.info('done (%s secs).' % (simrun.complete - simrun.started).seconds)
    
    except Exception as e:
        simrun.failed = timezone.now()
        simrun.fail_str = e.__str__()
        simrun.save()
        
        if e is ExitException:
            raise e
        
        logging.error('fail: %s (%s)' % (e.__str__(), type(e).__name__))

def work(threaded=True):
    ''' iterates non-started SimRun objects, updates statuses, and calls sim, layout display and plot functions '''
    
    # avoid having two worker threads starting on the same job
    simrun = get_and_start_new_simrun()
    
    while simrun:
        # exceptions raised during the processing block are written to the simrun object as fail, but do not break the processing loop
        try:
            
            if simrun.scanpoints == 1:
                logging.info('delegating simrun for %s...' % simrun.instr_displayname)
            else:
                logging.info('delegating simrun for %s (%d-point scansweep)...' % (simrun.instr_displayname, simrun.scanpoints))
            
            if threaded:
                t = threading.Thread(target=threadwork, args=(simrun,))
                t.setDaemon(True)
                t.setName('%s (%s)' % (t.getName().replace('Thread-','T'), simrun.instr_displayname))
                t.start()
            else:
                threadwork(simrun)
            
            # continue or cause a break iteration
        
        except Exception as e:
            if e is ExitException:
                raise e
            
            logging.error('fail: %s (%s)' % (e.__str__(), type(e).__name__))
        
        finally:
            simrun = get_and_start_new_simrun()
            if not simrun:
                logging.info("idle...")

class Command(BaseCommand):
    ''' django simrun processing command "runworker" '''
    help = 'generates mcstas/mxtrace simulation output and updates db'

    def add_arguments(self, parser):
        ''' adds the debug run option (run only once) '''
        parser.add_argument('--debug', action='store_true', help="runs work() only once")
        
    def handle(self, *args, **options):
        ''' implements main execution loop and debug run '''
        # enable logging 
        logging.basicConfig(level=logging.INFO,
                    format='%(threadName)-22s: %(message)s',
                    )
        
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
                work(threaded=False)
                exit()
            
            # main threaded execution loop:
            logging.info("looking for simruns...")
            while True:
                work()
                time.sleep(1)
            
        # ctr-c exits
        except KeyboardInterrupt:
            print("")
            logging.info("shutdown requested, exiting...")
            print("")
            print("")
        
        # handle exit-exception (programmatic shutdown)
        except ExitException as e:
            print("")
            logging.warning("exit exception raised, exiting (%s)" % e.__str__())
            print("")
            print("")

