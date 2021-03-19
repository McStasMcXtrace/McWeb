from __future__ import absolute_import
from django.contrib import admin
from .models import Signup, ContactEntry

admin.site.register(Signup)
admin.site.register(ContactEntry)
