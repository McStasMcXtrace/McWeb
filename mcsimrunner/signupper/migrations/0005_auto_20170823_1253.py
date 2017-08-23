# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('signupper', '0004_contactentry'),
    ]

    operations = [
        migrations.RenameField(
            model_name='signup',
            old_name='is_added',
            new_name='is_in_ldap',
        ),
        migrations.RemoveField(
            model_name='signup',
            name='added_ldap',
        ),
        migrations.RemoveField(
            model_name='signup',
            name='added_moodle',
        ),
        migrations.RemoveField(
            model_name='signup',
            name='is_limbo',
        ),
        migrations.RemoveField(
            model_name='signup',
            name='is_new',
        ),
        migrations.AddField(
            model_name='signup',
            name='deleted',
            field=models.DateTimeField(null=True, verbose_name=b'Deleted', blank=True),
        ),
        migrations.AddField(
            model_name='signup',
            name='is_self_signup',
            field=models.BooleanField(default=False),
        ),
    ]
