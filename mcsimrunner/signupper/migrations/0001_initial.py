# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models
import django.utils.timezone


class Migration(migrations.Migration):

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='Signup',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('username', models.CharField(max_length=200)),
                ('firstname', models.CharField(max_length=200)),
                ('lastname', models.CharField(max_length=200)),
                ('email', models.CharField(max_length=200)),
                ('password', models.CharField(max_length=200)),
                ('created', models.DateTimeField(default=django.utils.timezone.now, verbose_name=b'date created')),
                ('added_ldap', models.DateTimeField(null=True, verbose_name=b'date started', blank=True)),
                ('added_moodle', models.DateTimeField(null=True, verbose_name=b'date complete', blank=True)),
                ('notified', models.DateTimeField(null=True, verbose_name=b'date failed', blank=True)),
                ('fail_str', models.CharField(max_length=1000, null=True, blank=True)),
                ('courses_str', models.CharField(max_length=1000)),
            ],
        ),
    ]
