# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models
import django.utils.timezone


class Migration(migrations.Migration):

    dependencies = [
        ('signupper', '0001_initial'),
    ]

    operations = [
        migrations.AlterField(
            model_name='signup',
            name='added_ldap',
            field=models.DateTimeField(null=True, verbose_name=b'Added to ldap', blank=True),
        ),
        migrations.AlterField(
            model_name='signup',
            name='added_moodle',
            field=models.DateTimeField(null=True, verbose_name=b'Added to moodle', blank=True),
        ),
        migrations.AlterField(
            model_name='signup',
            name='created',
            field=models.DateTimeField(default=django.utils.timezone.now, verbose_name=b'Submitted'),
        ),
        migrations.AlterField(
            model_name='signup',
            name='notified',
            field=models.DateTimeField(null=True, verbose_name=b'Notified', blank=True),
        ),
    ]
