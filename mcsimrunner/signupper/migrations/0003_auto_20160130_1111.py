# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('signupper', '0002_auto_20160130_1101'),
    ]

    operations = [
        migrations.AddField(
            model_name='signup',
            name='is_added',
            field=models.BooleanField(default=False),
        ),
        migrations.AddField(
            model_name='signup',
            name='is_limbo',
            field=models.BooleanField(default=False),
        ),
        migrations.AddField(
            model_name='signup',
            name='is_new',
            field=models.BooleanField(default=True),
        ),
    ]
