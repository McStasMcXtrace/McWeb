# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('simrunner', '0003_remove_simrun_instr_filepath'),
    ]

    operations = [
        migrations.AddField(
            model_name='simrun',
            name='data_files_str',
            field=models.CharField(default=b'[]', max_length=2000),
        ),
    ]
