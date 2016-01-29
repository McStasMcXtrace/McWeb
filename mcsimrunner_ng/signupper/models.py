'''
signupper models
'''
from django.db.models import Model, CharField, DateTimeField
from django.utils import timezone
import json

class Signup(Model):
    ''' corresponds to a mcstas instrument contained in a certain folder '''
    username = CharField(max_length=200)
    firstname = CharField(max_length=200)
    lastname = CharField(max_length=200)
    email = CharField(max_length=200)
    password = CharField(max_length=200)
    
    created = DateTimeField('Submitted', default=timezone.now)
    added_ldap = DateTimeField('Added to ldap', blank=True, null=True)
    added_moodle = DateTimeField('Added to moodle', blank=True, null=True)
    notified = DateTimeField('Notified', blank=True, null=True)
    
    fail_str = CharField(max_length=1000, blank=True, null=True)
    
    courses_str = CharField(max_length=1000)
    @property
    def courses(self):
        return json.loads(self.params_str)
    @courses.setter
    def courses(self, p):
        self.params_str = json.dumps(p)
    
    def is_new(self):
        return (self.added_ldap is None) and (self.added_moodle is None) and (self.notified is None) and (self.fail_str is None)
    
    def is_added(self):
        return self.added_ldap and self.added_moodle and self.notified
    
    def is_limbo(self):
        return not self.is_new() and self.is_added()
    
    def status(self):
        if self.is_new():
            return 'New'
        elif self.is_added():
            return 'Added'
        else:
            return 'Limbo'
    
    def __str__(self):
        return "%s" % (self.username)
