#!/usr/bin/env python
import os
import sys

if __name__ == "__main__":
    # enables imports from modules (folder containing __init__.py) one level up from this
    sys.path.append('..')
    
    # import our django settings file
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "mcweb.settings")
    
    from django.core.management import execute_from_command_line

    execute_from_command_line(sys.argv)
