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
    is_self_signup = BooleanField(default=False)
    
    fail_str = CharField(max_length=1000, blank=True, null=True)
    
    courses_str = CharField(max_length=1000)
    @property
    def courses(self):
        return json.loads(self.courses_str)
    @courses.setter
    def courses(self, c):
        self.courses_str = json.dumps(c)
    
    def __str__(self):
        return "%s" % (self.username)

class ContactEntry(Model):
    ''' contact entry and related context info '''
    replyto = CharField(max_length=200)
    text = TextField()
    
    created = DateTimeField('Submitted', default=timezone.now)
    delivered = DateTimeField('Admin notified of this entry', blank=True, null=True)
    
    fail_str = CharField(max_length=1000, blank=True, null=True)
