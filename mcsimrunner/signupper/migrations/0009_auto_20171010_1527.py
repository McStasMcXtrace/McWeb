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
            field=models.CharField(default=b'', max_length=200),
        ),
        migrations.AddField(
            model_name='signup',
            name='institution_type',
            field=models.CharField(default=b'', max_length=200),
        ),
        migrations.AddField(
            model_name='signup',
            name='research_interest',
            field=models.CharField(default=b'', max_length=200),
        ),
    ]
