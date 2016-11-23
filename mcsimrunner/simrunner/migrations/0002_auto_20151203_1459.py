# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('simrunner', '0001_initial'),
    ]

    operations = [
        migrations.AddField(
            model_name='simrun',
            name='plot_files_log_str',
            field=models.CharField(default=b'[]', max_length=2000),
        ),
        migrations.AlterField(
            model_name='simrun',
            name='plot_files_str',
            field=models.CharField(default=b'[]', max_length=2000),
        ),
    ]
