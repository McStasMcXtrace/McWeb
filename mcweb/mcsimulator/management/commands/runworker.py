from django.core.management.base import BaseCommand
from django.db import transaction

import tarfile
import traceback
import time, os, re
import simplejson as json
from subprocess import Popen, PIPE
from os.path import basename, dirname, splitext, abspath

from mcsimulator.models import SimRun
from common import fetch, one_or_none
from mcwww.settings import ( IMAGE_FORMAT, MPI_NP, DATA_FILES,
                             MAX_RAY_SAMPLES, MAX_SCAN_POINTS )

#================#
# Module Methods #
#================#
def popenwait(args, cwd, log):
    pid = Popen(args, cwd=cwd, stdout=PIPE, stderr=PIPE)
    out, err = pid.communicate()
    log(out, err)
    return out

def mcplot(simfile, outfile, logger, logy=False):
    ''' Plot a mccode.sim file with mcplot '''
    args = ( ["mcplot", "-%s" % IMAGE_FORMAT] + (logy and ["-log"] or []) + [basename(simfile)] )
    popenwait(args, cwd=dirname(simfile), log=logger)
    os.rename("%s.%s" % (simfile, IMAGE_FORMAT) , outfile)
    



def display(instr, params, outfile, log, fmt=IMAGE_FORMAT):
    ''' Display instrument '''
    # VRML needs --format, which does not seem to work with gif/ps   MARK NOTE: this may be a good idea to look into.
    fmt_arg = fmt == 'vrml' and '--format=vrml' or '-'+fmt
    args = (["mcdisplay", "-k", "--save", fmt_arg, basename(instr),
             "-n", str(1) # precision (iterations)
             ] + params)
    popenwait(args, cwd=dirname(instr), log=log)
    # VRML needs special treatment
    if fmt == 'vrml':
        mcout = '%s/%s' % (dirname(instr), 'mcdisplay_commands.wrl')
    else:
        mcout = splitext(instr)[0] + ".out." + fmt
    os.rename(mcout, outfile)


def value_to_str(val):
    ''' Converts a value to string, handling lists specially '''
    if isinstance(val, list):
        return ','.join(value_to_str(x) for x in val)
    return str(val)


def first_range(val):
    ''' Select the first element of a range, e.g. lambda=0.7,1.2 -> lambda=0.7 '''
    return [ re.sub(r',[^\s]+', r'', x) for x in val ]


def work():
    ''' Fetch and process a job by running the McStas simulation '''
    # fetch job
    run = one_or_none(fetch(SimRun, status="waiting").order_by('created'))
    if run is None:
        return

    run.status = "running"
    run.save()

    print "Running job: ", run.ref

    # create output folder; works as a lock
    workdir = 'out/%s' % run.ref
    try:
        os.mkdir(workdir)
    except OSError:
        # Someone else beat us to it, bail out
        print "Skipping: already running."
        return

    # try to process job
    try:
        processJob(run, workdir)
    except:
        exc = traceback.format_exc()
        file(os.path.join(workdir, 'err.txt'), 'a').write('\n' + '''Python stack-trace:\n%s''' % exc)
        print exc

    # mark job as completed
    run.status = "done"
    with transaction.atomic():
        run.save()
    print "Done."



def processJob(run, workdir):
    ''' Process a specific job '''

    def appendLog(out, err):
        # populate result folder
        if out: file(os.path.join(workdir, "out.txt"), "a").write(out)
        if err: file(os.path.join(workdir, "err.txt"), "a").write(err)

    # pick seed and samples
    params = run.params

    seed = params["_seed"]
    samples = min(MAX_RAY_SAMPLES, params["_samples"])
    npoints = min(MAX_SCAN_POINTS, params["_npoints"])

    # instrument name
    name = run.sim.name
    group = run.sim.simgroup
    siminstr = "sim/"+group+"/"+name+".instr" #SIM_SRC_PATH % group % name
    simbin = "sim/"+group+"/"+name+".out"
    simc = "sim/"+group+"/"+name+".c"
    simhtml = "sim/"+name+".html"

    name=basename(name)
    
    # Create hard links to instrument source, c-code and binary
    for path in (siminstr, simbin, simc, simhtml):
        os.link(path, os.path.join(workdir, basename(path)))

    # Create soft links to data files/folders
    for path in DATA_FILES:
        for fname in os.listdir(path):
            os.symlink('%s/%s' % (abspath(path), fname), os.path.join(workdir, fname))

    # compute instrument layout
    is_scan = any(isinstance(param, list) for param in params.values())

    # generate list of parameters
    params_strs = [ '%s=%s' % (str(k), value_to_str(v)) for k, v in params.items()
                    if not k[0] == '_' ]

    # Generate instrument graphics with mcdisplay
    display(os.path.join(workdir, (name + ".instr")), first_range(params_strs),
            os.path.join(workdir, "layout.png"), log=appendLog) #changed gif -> png
    display(os.path.join(workdir, (name + ".instr")), first_range(params_strs),
            os.path.join(workdir, "layout.wrl"), log=appendLog, fmt='vrml')

    # run mcstas via mcrun
    args = ["mcrun"] + \
           (seed > 0   and ["--seed", str(seed)]    or []) + \
           (is_scan    and ["-N",     str(npoints)] or []) + \
           (MPI_NP > 0 and ["--mpi=%s" % MPI_NP]    or []) + \
           ["--ncount", str(samples),
            "--dir",    "mcstas",
            name] + params_strs
    popenwait(args, cwd=workdir, log=appendLog)

    # tar results
    tarname = 'mcstas-' + run.ref
    tarf = os.path.join(workdir, ('%s.tar.gz' % tarname))
    tarfile.open(tarf, 'w:gz').add(os.path.join(workdir, "mcstas"), arcname=tarname)

    def process_components(sim_path):
        if os.path.isfile(sim_path):
            # Dump components
            comps = re.findall(r'filename:[ \t]*([^\s]+)',
                               file(sim_path).read())
            file(dirname(sim_path) + "/comps.json", "w").write(json.dumps(comps))
        # plot components
            for comp in comps:
                for mode in ("lin", "log"):
                    plot(dirname(sim_path) + '/' + comp,
                         outfile=dirname(sim_path) + ('/plot-%s-%s.png' % (comp, mode)), # gif -> png
                         logger=appendLog,
                         logy=(mode == "log"))
    
    os.path.walk(os.path.join(workdir, 'mcstas'),
                 lambda _arg, folder, files:
                 process_components(folder + "/mccode.sim"),
                 [])
    os.path.walk(os.path.join(workdir, 'mcstas'),
                 lambda _arg, folder, files:
                 process_components(folder + "/mcstas.sim"),
                 [])

#===================#
# runworker Command #
#===================#
# try to use new R plotter instead of mcplot
try:
    from rplot.plot import plotSim
    def plot(simfile, outfile, logger, logy=False):
        ''' Plot a sim file with R '''
        try:
            plotSim(simfile, logy, IMAGE_FORMAT)
            os.rename('%s.%s' % (simfile, IMAGE_FORMAT), outfile)
        except Exception,e:
            print 'Fallback to mcplot, because:'
            traceback.print_exc()
            mcplot(simfile, outfile, logger=logger, logy=logy)
    print 'INFO: using plotter from rplot/'
except Exception, e:
    # Error occured: Print it and fallback to old mcplot
    print e
    plot = mcplot


class Command(BaseCommand):
    def handle(self, *args, **options):
        while True:
            time.sleep(1)
            try: 
                work()
            except:
                print 'Job failed: '
                traceback.print_exc()

