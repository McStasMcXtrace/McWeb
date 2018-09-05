# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('simrunner', '0008_auto_20180904_1510'),
    ]

    operations = [
        migrations.AddField(
            model_name='instrument',
            name='always_simulate',
            field=models.BooleanField(default=False),
        ),
    ]
