# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models
import django.utils.timezone


class Migration(migrations.Migration):

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='InstrGroup',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('name', models.CharField(unique=True, max_length=200)),
            ],
        ),
        migrations.CreateModel(
            name='Instrument',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('name', models.CharField(unique=True, max_length=200)),
                ('displayname', models.CharField(max_length=200)),
                ('docs', models.TextField()),
                ('params_str', models.CharField(max_length=1000)),
                ('group', models.ForeignKey(to='simrunner.InstrGroup')),
            ],
        ),
        migrations.CreateModel(
            name='SimRun',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('owner_username', models.CharField(max_length=200, null=True, blank=True)),
                ('group_name', models.CharField(max_length=200, null=True, blank=True)),
                ('instr_displayname', models.CharField(max_length=200, null=True, blank=True)),
                ('neutrons', models.PositiveIntegerField(default=1000000)),
                ('seed', models.PositiveIntegerField(default=0)),
                ('scanpoints', models.PositiveIntegerField(default=1)),
                ('params_str', models.CharField(max_length=1000)),
                ('created', models.DateTimeField(default=django.utils.timezone.now, verbose_name=b'date created')),
                ('started', models.DateTimeField(null=True, verbose_name=b'date started', blank=True)),
                ('complete', models.DateTimeField(null=True, verbose_name=b'date complete', blank=True)),
                ('failed', models.DateTimeField(null=True, verbose_name=b'date failed', blank=True)),
                ('fail_str', models.CharField(max_length=1000, null=True, blank=True)),
                ('data_folder', models.CharField(max_length=200, null=True, blank=True)),
                ('instr_filepath', models.CharField(max_length=200, null=True, blank=True)),
                ('plot_files_str', models.CharField(default=b'[]', max_length=1000)),
            ],
        ),
    ]
