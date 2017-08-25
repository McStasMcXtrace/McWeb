# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('signupper', '0005_auto_20170823_1253'),
    ]

    operations = [
        migrations.AddField(
            model_name='signup',
            name='is_in_moodle',
            field=models.BooleanField(default=False),
        ),
        migrations.AlterField(
            model_name='signup',
            name='fail_str',
            field=models.CharField(default=b'', max_length=1000),
        ),
    ]
