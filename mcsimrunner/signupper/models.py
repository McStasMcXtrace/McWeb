'''
signupper models
'''
from django.db.models import Model, CharField, DateTimeField, TextField
from django.utils import timezone
import json
from django.db.models import BooleanField


class Signup(Model):
    username = CharField(max_length=200)
    firstname = CharField(max_length=200)
    lastname = CharField(max_length=200)
    email = CharField(max_length=200)
    password = CharField(max_length=200)
    
    created = DateTimeField('Submitted', default=timezone.now)
    notified = DateTimeField('Notified', blank=True, null=True)
    deleted = DateTimeField('Deleted', blank=True, null=True)
    
    is_in_ldap = BooleanField(default=False)
    is_in_moodle = BooleanField(default=False)
    is_self_signup = BooleanField(default=False)
    
    fail_str = CharField(max_length=1000, default='')
    
    courses_str = CharField(max_length=1000)
    @property
    def courses(self):
        return json.loads(self.courses_str)
    @courses.setter
    def courses(self, c):
        self.courses_str = json.dumps(c)
    
    def __str__(self):
        return "%s" % (self.username)
    
    def get_states(self):
        return ['limbo', 'new', 'new self-signups', 'added', 'deleted', 'disabled']
    
    def state(self):
        '''
        0 - limbo
        1 - new
        2 - new self-signups
        3 - added
        4 - deleted
        5 - disabled
        '''
        ldap = self.is_in_ldap
        moo = self.is_in_moodle
        slf = self.is_self_signup
        deltd = self.deleted is not None
        
        if not ldap and not moo and not self and not deltd:
            return 1
        elif not ldap and not moo and self and not deltd:
            return 2
        elif ldap and moo and not deltd:
            return 3
        elif not ldap and not moo and deltd:
            return 4
        elif not ldap and moo and not deltd:
            return 5
        else:
            return 0

class ContactEntry(Model):
    ''' contact entry and related context info '''
    replyto = CharField(max_length=200)
    text = TextField()
    
    created = DateTimeField('Submitted', default=timezone.now)
    delivered = DateTimeField('Admin notified of this entry', blank=True, null=True)
    
    fail_str = CharField(max_length=1000, blank=True, null=True)
