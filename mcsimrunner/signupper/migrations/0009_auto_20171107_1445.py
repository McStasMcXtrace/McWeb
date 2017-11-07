# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('signupper', '0008_auto_20170907_0842'),
    ]

    operations = [
        migrations.AddField(
            model_name='signup',
            name='country',
            field=models.CharField(max_length=200, blank=True),
        ),
        migrations.AddField(
            model_name='signup',
            name='description',
            field=models.CharField(max_length=200, blank=True),
        ),
        migrations.AddField(
            model_name='signup',
            name='institution',
            field=models.CharField(max_length=200, blank=True),
        ),
    ]
