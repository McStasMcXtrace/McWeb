'''
simrunner models
'''
from django.db.models import Model, CharField, TextField, ForeignKey, DateTimeField, PositiveIntegerField
from django.utils import timezone
import json

class InstrGroup(Model):
    ''' corresponds to a folder containing instruments '''
    name = CharField(max_length=200, unique=True)
    
    def __str__(self):
        return self.name

class Instrument(Model):
    ''' corresponds to a mcstas instrument contained in a certain folder '''
    group = ForeignKey(InstrGroup)
    
    name = CharField(max_length=200, unique=True)
    displayname = CharField(max_length=200)
    docs = TextField()
    
    params_str = CharField(max_length=1000)

    image = CharField(max_length=200)

    infostring = CharField(max_length=1024)
    
    @property
    def params(self):
        return json.loads(self.params_str)
    
    @params.setter
    def params(self, p):
        self.params_str = json.dumps(p)
    
    def __str__(self):
        return self.displayname

class SimRun(Model):
    ''' corresponds to a simulation run of a particular instrument - but SimRun is decoupled from the rest of the db '''
    owner_username = CharField(max_length=200, blank=True, null=True)
    group_name = CharField(max_length=200, blank=True, null=True)
    instr_displayname = CharField(max_length=200, blank=True, null=True)
    
    neutrons = PositiveIntegerField(default=1000000)
    seed = PositiveIntegerField(default=0)
    scanpoints = PositiveIntegerField(default=1)
    params_str = CharField(max_length=1000)
    
    # meta-fields below this line
    created = DateTimeField('date created', default=timezone.now)
    started = DateTimeField('date started', blank=True, null=True)
    complete = DateTimeField('date complete', blank=True, null=True)
    failed = DateTimeField('date failed', blank=True, null=True)
    fail_str = CharField(max_length=1000, blank=True, null=True)
    
    data_folder = CharField(max_length=200, blank=True, null=True)
    plot_files_str = CharField(max_length=2000, default='[]')
    plot_files_log_str = CharField(max_length=2000, default='[]')
    data_files_str = CharField(max_length=2000, default='[]')
    
    @property
    def plot_files(self):
        return json.loads(self.plot_files_str)
    @plot_files.setter
    def plot_files(self, pf):
        self.plot_files_str = json.dumps(pf)
    
    @property
    def plot_files_log(self):
        return json.loads(self.plot_files_log_str)
    @plot_files_log.setter
    def plot_files_log(self, pf):
        self.plot_files_log_str = json.dumps(pf)
    
    @property
    def data_files(self):
        return json.loads(self.data_files_str)
    @data_files.setter
    def data_files(self, df):
        self.data_files_str = json.dumps(df)
    
    @property
    def params(self):
        return json.loads(self.params_str)
    @params.setter
    def params(self, p):
        self.params_str = json.dumps(p)
        
    def status(self):
        if self.complete:
            return 'complete'
        elif self.failed:
            return 'error'
        elif self.started:
            return 'running'
        else:
            return 'init'
    
    def __str__(self):
        return "%s_%s_%s" % (self.owner_username, self.instr_displayname, str(self.created.strftime("%Y%m%d_%H%M%S")))
