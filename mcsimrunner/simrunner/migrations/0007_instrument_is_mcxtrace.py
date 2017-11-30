# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('simrunner', '0006_simrun_gravity'),
    ]

    operations = [
        migrations.AddField(
            model_name='instrument',
            name='is_mcxtrace',
            field=models.BooleanField(default=False),
        ),
    ]
