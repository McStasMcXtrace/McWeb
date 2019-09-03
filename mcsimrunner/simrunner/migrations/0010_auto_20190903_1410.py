# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('simrunner', '0009_instrument_always_simulate'),
    ]

    operations = [
        migrations.RenameField(
            model_name='simrun',
            old_name='was_run',
            new_name='enable_cachefrom',
        ),
    ]
