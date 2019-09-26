'''
simrunner functional views
'''
from django.utils import timezone
from django.contrib.auth import authenticate, login, logout
from django.shortcuts import render, redirect
from django.contrib.auth.decorators import login_required
from django.http import HttpResponse
from os.path import basename
from models import InstrGroup, Instrument, SimRun
import json
from django.views.decorators.clickjacking import xframe_options_exempt
from mcweb.settings import DEFAULT_GROUP, DEFAULT_INSTR
@xframe_options_exempt

def home(req):
    nxt = req.GET.get('next', None)
    if nxt:
        req.session['next'] = nxt
    return render(req, template_name='login.html')

def login_post(req):
    form = req.POST
    username = form.get('username', '')
    password = form.get('password', '')
    
    user = authenticate(username=username, password=password)
    if user is None or not user.is_active:
        return redirect(home)
    
    login(req, user)
    
    nxt = req.session.get('next', None)
    if nxt:
        req.session.pop('next', None)
        return redirect(nxt)
    
    group = DEFAULT_GROUP
    instr = DEFAULT_INSTR
    
    return redirect('instrument', group_name=group, instr_name=instr)

def logout_user(req):
    logout(req)
    
    return redirect(home)

def login_status(req):
    ''' returns a tiny "you are loggged in as..." type text intended to be embedded in other web pages '''
    if not req.user.is_authenticated():
        #return HttpResponse('You are not logged in.')
        return render(req, 'login_slim.html')
    else:
        return render(req, 'logout_slim.html')
        #return HttpResponse('You are logged in as %s.' % req.user)

def login_slim_post(req):
    ''' post a login request that should be redirected to the login_status page '''
    
    form = req.POST
    username = form.get('username', '')
    password = form.get('password', '')
    
    user = authenticate(username=username, password=password)
    if user is None or not user.is_active:
        return redirect('/loginstatus/')
    
    login(req, user)
    
    return redirect('/loginstatus/')

def logout_slim(req):
    ''' log out and redirect to login_status '''
    logout(req)
    
    return HttpResponse('Not logged in')

@login_required
def recent(req):
    ''' returns a link to recent simruns '''
    all_simruns = SimRun.objects.filter(owner_username=req.user)
    
    # make sure we only work on instances with valied data_folder atributes
    all_simruns = filter(lambda s: s.data_folder is not None, all_simruns)
    
    # create a list of template-friendly records from the simruns belonging to this user 
    datafolder_simname = map(lambda s: [s.data_folder, basename(s.data_folder)], all_simruns)
    
    # most recent simruns first 
    datafolder_simname = reversed(datafolder_simname)
    
    return render(req, 'recent.html', {'datafolder_simname': datafolder_simname})

@login_required
def instrument_menu(req, group_name, instr_name=None):
    ''' redirects to instrument, setting menu=True '''
    return instrument(req, group_name=group_name, instr_name=instr_name, menu=True)

@login_required
def instrument(req, group_name, instr_name=None, menu=False):
    ''' displays the instrument page, optionally with groups and instruments visible '''
    group = InstrGroup.objects.get(name=group_name)
    
    # TODO: move the case of instr_name=None to another view, due to the need to render instrument params
    if instr_name == None:
        instr_name = Instrument.objects.filter(group=group)[0].displayname
    
    instr = Instrument.objects.get(name=group_name + '_' + instr_name)
    
    grpinstr_style = "display:none;"
    instr_urlbit = "instrument"
    
    if menu:
        grpinstr_style = ""
        instr_urlbit = "instrument-menu"
    
    # collect properties
    group_names = map(lambda g: g.name, InstrGroup.objects.all())
    instr_displaynames = map(lambda i: i.displayname, Instrument.objects.filter(group=group))
    params = instr.params
    neutrons = 1000000
    seed = 0
    scanpoints = 1
    
    return render(req, 'instrument.html', { 'group_names': group_names, 'instr_displaynames': instr_displaynames, 'group_name': group.name, 'instr_displayname': instr.displayname,
                                            'instr_image': instr.image,
                                            'scanpoints': scanpoints, 'neutrons': neutrons, 'seed': seed, 'params': params, 'params_jsonified': json.dumps(params),
                                            'show_menu': menu, 'instr_urlbit': instr_urlbit, 
                                            'gravity_visible' : not instr.is_mcxtrace, 'force_run' : instr.always_simulate })

@login_required    
def instrument_post(req):
    form = req.POST
    
    owner_username = req.user.username
    group_name = form.get('group_name')
    instr_displayname = form.get('instr_displayname')
    
    neutrons = int(float(form.get('neutrons')))
    scanpoints = form.get('scanpoints')
    seed = form.get('seed')
    gravity = bool(form.get('gravity'))
    recalc = bool(form.get('force_recalc'))

    params_default = json.loads(form.get('params_jsonified'))
    params=[]
    for p in params_default: 
        val = form.getlist(p[0])[0]
        val = val.replace(' ', '')
        params.append([p[0], val])
    
    simrun = SimRun(group_name=group_name, instr_displayname=instr_displayname, 
                    owner_username=owner_username,
                    neutrons=neutrons, scanpoints=scanpoints, seed=seed, gravity=gravity,
                    params=params, force_run=recalc)
    simrun.save()
    return redirect('simrun', sim_id=simrun.id)

@login_required
def simrun(req, sim_id):
    ''' Simulation run waiting page. '''
    simrun = SimRun.objects.get(id=sim_id)
    
    if simrun.failed:
        # TODO: make a static fail html page also named browse.html
        # TODO: ensure static page generation only happens once
        return render(req, 'fail.html', {'instr_displayname': simrun.instr_displayname, 'fail_str': simrun.fail_str, 'data_folder' : simrun.data_folder})

    elif simrun.complete:
        # redirect to static
        return redirect('/%s/browse.html' % simrun.data_folder)
    
    # simrun live status 
    else:
        return render(req, 'status.html', {'group_name': simrun.group_name, 'instr_displayname': simrun.instr_displayname,
                                           'neutrons': simrun.neutrons, 'seed': simrun.seed,
                                           'scanpoints': simrun.scanpoints, 'params': simrun.params,
                                           'status': simrun.status, 'date_time_created': timezone.localtime(simrun.created).strftime("%H:%M:%S"),
                                           'data_folder' : simrun.data_folder})

