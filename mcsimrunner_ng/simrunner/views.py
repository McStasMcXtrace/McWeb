'''
simrunner functional views
'''
from django.utils import timezone
from django.contrib.auth import authenticate, login, logout
from django.shortcuts import render, redirect
from django.contrib.auth.decorators import login_required
from os.path import basename
from models import InstrGroup, Instrument, SimRun
from mcweb.settings import USE_AOPT
import json
from generate_static import McStaticDataBrowserGenerator
from django.views.decorators.clickjacking import xframe_options_exempt
@xframe_options_exempt

def home(req):
    return render(req, template_name='login.html')

def login_post(req):
    form = req.POST
    username = form.get('username', '')
    password = form.get('password', '')
    
    user = authenticate(username=username, password=password)
    if user is None or not user.is_active:
        return redirect(home)
    
    login(req, user)

    # TODO: enable default group and instrument on user object
    default_group = 'group1'
    default_instr = 'PSI_DMC'
    
    return redirect('instrument', group_name=default_group, instr_name=default_instr)

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
    
    return redirect('/loginstatus/')

@login_required
def recent(req):
    ''' returns a link to recent simruns '''
    all_simruns = SimRun.objects.filter(owner_username=req.user)
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
    
    return render(req, 'instrument.html', {'group_names': group_names, 'instr_displaynames': instr_displaynames, 'group_name': group.name, 'instr_displayname': instr.displayname,
                                           'instr_image': instr.image,
                                           'scanpoints': scanpoints, 'neutrons': neutrons, 'seed': seed, 'params': params, 'params_jsonified': json.dumps(params),
                                           'grpinstr_style': grpinstr_style, 'instr_urlbit': instr_urlbit})

@login_required    
def instrument_post(req):
    form = req.POST
    
    owner_username = req.user.username
    group_name = form.get('group_name')
    instr_displayname = form.get('instr_displayname')
    
    neutrons = form.get('neutrons')
    scanpoints = form.get('scanpoints')
    seed = form.get('seed')
    
    params_default = json.loads(form.get('params_jsonified'))
    params=[]
    for p in params_default: 
        params.append([p[0], form.getlist(p[0])[0]])
    
    simrun = SimRun(group_name=group_name, instr_displayname=instr_displayname, 
                    owner_username=owner_username,
                    neutrons=neutrons, scanpoints=scanpoints, seed=seed,
                    params=params)
    simrun.save()
    return redirect('simrun', sim_id=simrun.id)

@login_required
def simrun(req, sim_id):
    ''' "%Y-%m-%d_%H:%M:%S" '''
    simrun = SimRun.objects.get(id=sim_id)
    
    if simrun.failed:
        # TODO: make a static fail html page also named browse.html 
        # TODO: ensure static page generation only happens once
        return render(req, 'fail.html', {'instr_displayname': simrun.instr_displayname, 'fail_str': simrun.fail_str, 'data_folder' : simrun.data_folder})
    
    if simrun.complete:
        # apply config of 3d instrument layout browser
        if USE_AOPT:
            iframestyle = ""
        else:
            iframestyle = "display:none"
        
        # generate data browser (TODO: make sure static page generation only happens once)
        lin_log_html = 'lin_log_url: impl.'
        gen = McStaticDataBrowserGenerator()
        gen.set_base_context({'group_name': simrun.group_name, 'instr_displayname': simrun.instr_displayname, 'date_time_completed': timezone.localtime(simrun.complete).strftime("%H:%M:%S, %d/%m-%Y"),
                              'params': simrun.params, 'neutrons': simrun.neutrons, 'seed': simrun.seed, 'scanpoints': simrun.scanpoints,
                              'lin_log_html': lin_log_html,
                              'data_folder': simrun.data_folder, 'iframestyle': iframestyle})

        if simrun.scanpoints == 1:
            gen.generate_browsepage(simrun.data_folder, simrun.plot_files, simrun.data_files)
        else:
            gen.generate_browsepage_sweep(simrun.data_folder, simrun.plot_files, simrun.data_files, simrun.scanpoints)
        # redirect to static
        return redirect('/%s/browse.html' % simrun.data_folder)
    
    # simrun live status 
    return render(req, 'status.html', {'group_name': simrun.group_name, 'instr_displayname': simrun.instr_displayname, 'neutrons': simrun.neutrons, 'seed': simrun.seed,
                                       'scanpoints': simrun.scanpoints, 'params': simrun.params,
                                       'status': simrun.status, 'date_time_created': timezone.localtime(simrun.created).strftime("%H:%M:%S")})
