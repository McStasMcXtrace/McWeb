# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models
import django.utils.timezone


class Migration(migrations.Migration):

    dependencies = [
        ('signupper', '0003_auto_20160130_1111'),
    ]

    operations = [
        migrations.CreateModel(
            name='ContactEntry',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('replyto', models.CharField(max_length=200)),
                ('text', models.TextField()),
                ('created', models.DateTimeField(default=django.utils.timezone.now, verbose_name=b'Submitted')),
                ('delivered', models.DateTimeField(null=True, verbose_name=b'Admin notified of this entry', blank=True)),
                ('fail_str', models.CharField(max_length=1000, null=True, blank=True)),
            ],
        ),
    ]
