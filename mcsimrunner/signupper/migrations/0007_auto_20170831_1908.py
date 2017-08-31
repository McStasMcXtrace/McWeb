# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('signupper', '0006_auto_20170825_1647'),
    ]

    operations = [
        migrations.AlterField(
            model_name='signup',
            name='fail_str',
            field=models.CharField(max_length=1000, blank=True),
        ),
    ]
