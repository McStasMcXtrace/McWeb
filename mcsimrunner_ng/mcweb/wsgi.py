"""
WSGI config for mcweb project.

It exposes the WSGI callable as a module-level variable named ``application``.

For more information on this file, see
https://docs.djangoproject.com/en/1.8/howto/deployment/wsgi/
"""

import os
import sys

# enables imports from modules (folder containing __init__.py) one level up from this
sys.path.append('..')

# import our django settings file
from django.core.wsgi import get_wsgi_application

os.environ.setdefault("DJANGO_SETTINGS_MODULE", "mcweb.settings")

application = get_wsgi_application()
