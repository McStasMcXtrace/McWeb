# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('signupper', '0007_auto_20170831_1908'),
    ]

    operations = [
        migrations.AlterField(
            model_name='signup',
            name='courses_str',
            field=models.CharField(default=b'[]', max_length=1000),
        ),
    ]
