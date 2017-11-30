# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('simrunner', '0005_auto_20151204_2042'),
    ]

    operations = [
        migrations.AddField(
            model_name='simrun',
            name='gravity',
            field=models.BooleanField(default=False),
        ),
    ]
