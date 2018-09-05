# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('simrunner', '0007_instrument_is_mcxtrace'),
    ]

    operations = [
        migrations.AddField(
            model_name='simrun',
            name='force_run',
            field=models.BooleanField(default=False),
        ),
        migrations.AddField(
            model_name='simrun',
            name='was_run',
            field=models.BooleanField(default=False),
        ),
    ]
