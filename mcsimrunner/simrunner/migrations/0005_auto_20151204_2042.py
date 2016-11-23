# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('simrunner', '0004_simrun_data_files_str'),
    ]

    operations = [
        migrations.AddField(
            model_name='instrument',
            name='image',
            field=models.CharField(default=' ', max_length=200),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='instrument',
            name='infostring',
            field=models.CharField(default=' ', max_length=1024),
            preserve_default=False,
        ),
    ]
