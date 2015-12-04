# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('simrunner', '0002_auto_20151203_1459'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='simrun',
            name='instr_filepath',
        ),
    ]
