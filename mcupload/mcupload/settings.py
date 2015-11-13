from os import path
import ldap
from django_auth_ldap.config import LDAPSearch

BASE_DIR = path.dirname(path.dirname(__file__))
#=============#
# LDAP Config #
#=============#
AUTH_LDAP_SERVER_URI = "ldap://localhost"
AUTH_LDAP_BIND_DN = ""
AUTH_LDAP_BIND_PASSWORD = ""
AUTH_LDAP_USER_SEARCH = LDAPSearch("ou=users,dc=fysik,dc=dtu,dc=dk", ldap.SCOPE_SUBTREE, "(uid=%(user)s)")
AUTHENTICATION_BACKENDS = (
    'django_auth_ldap.backend.LDAPBackend',     # this line implements the LDAP sign in
    #'django.contrib.auth.backends.ModelBackend', # uncomment this line to enable local sign-on (django-db)
)
AUTH_USER_MODEL = ('auth.User')

#===============#
# Django Config #
#===============#
# DJANGO_SETTINGS_MODULE = 'settings.py'
SECRET_KEY     = 'gaeh@565h%=7)gw#625*ag82am#*55xnb40xa769yaxq-^ukj*'   # THIS SHOULD BE SECRET! REGENERATE IT A LOT BECAUSE OF THE SERIALIZER USED. ----^
ALLOWED_HOSTS  = []
DEBUG          = True # False                <----  THIS NEEDS TO BE SET TO FALSE. (css broken if false!!!!)
INSTALLED_APPS = (
    # mcApps #
    # ------ #
    'mcupload',
    # django Apps #
    # ----------- #
    # Admin Site
    'django.contrib.admin',
    'django.contrib.auth',
    'django.contrib.contenttypes',
    # Std
    'django.contrib.sessions',
    'django.contrib.messages',
    'django.contrib.staticfiles',
)
MIDDLEWARE_CLASSES = (
    'django.middleware.common.CommonMiddleware',
    'django.contrib.sessions.middleware.SessionMiddleware',
    'django.middleware.csrf.CsrfViewMiddleware',
    # Admin Site #
    'django.contrib.auth.middleware.AuthenticationMiddleware',
    'django.contrib.messages.middleware.MessageMiddleware',
    'django.middleware.clickjacking.XFrameOptionsMiddleware',
)
FILE_UPLOAD_PERMISSIONS = 0644
#-----------------#
# DATABASE CONFIG #
#-----------------#
ADMINS     = ( 
    ('Peter Willendrup', 'pkwi@fysik.dyu.dk'),
    ('Mark Lewis', 'lewis@fysik.dtu.dk'),
    )
MANAGERS   = ADMINS
DB_LOCATION = '../DB/mcwww.sqlite3'
DATABASES  = {
    'default': { 'ENGINE': 'django.db.backends.sqlite3',
                 'NAME': path.join(BASE_DIR, DB_LOCATION),
                 }
    }
#---------------#
# END DATABASES #
#---------------#

#--------------#
# FILE SERVING # 
#--------------#
WSGI_APPLICATION = 'mcupload.wsgi.application' # path to WSGI app used by runserver.
ROOT_URLCONF     = 'mcupload.urls'             # File holding URLs to be served.
TEMPLATES = [{'BACKEND': 'django.template.backends.django.DjangoTemplates',
              'DIRS': [BASE_DIR + '/templates/'],
              'OPTIONS': { 'context_processors': ['django.template.context_processors.debug',
                                                  'django.template.context_processors.request',
                                                  'django.contrib.auth.context_processors.auth',
                                                  'django.contrib.messages.context_processors.messages',
                                                  ],
                           'loaders': ['django.template.loaders.filesystem.Loader',
                                       'django.template.loaders.app_directories.Loader'
                                       ],
                           'debug': True
                           },
              }, ]
STATIC_ROOT      = ''                       # Store custom static files in apps/static/ subdirectories and in STATICFILES_DIRS.
STATIC_URL       = '/static/'
STATICFILES_DIRS = (
    BASE_DIR + '/static/',
    BASE_DIR + '/out/'
)
STATICFILES_FINDERS = (
    'django.contrib.staticfiles.finders.FileSystemFinder',
    'django.contrib.staticfiles.finders.AppDirectoriesFinder',
)
#------------------#
# END FILE SERVING # 
#------------------#
#============================#
# END (USEFUL) DJANGO CONFIG #
#============================#

#-----------#
# STD STUFF #
#-----------#
TIME_ZONE     = 'Europe/Copenhagen'  # Local time zone. See http://en.wikipedia.org/wiki/List_of_tz_zones_by_name
LANGUAGE_CODE = 'en-gb'              # Language code. See http://www.i18nguy.com/unicode/language-identifiers.html
SITE_ID       = 1
USE_I18N      = True                 # loading internationalization machinery. (default -> true, optimised -> false... don't change)
USE_L10N      = True                 # locale date formatting
USE_TZ        = True                 # timezone-aware datetimes.
#----------------#
# logging config #
#----------------#
# this config:
#   filter
#    DEBUG=False.
#   handled by
#    email site admins 
#   perform on
#    HTTP 500 error 
LOGGING = { # See http://docs.djangoproject.com/en/dev/topics/logging 
    'version': 1,
    'disable_existing_loggers': False,
    'filters': {'require_debug_false':  {'()': 'django.utils.log.RequireDebugFalse' 
                                         }
                },
    'handlers': {'mail_admins': {'level': 'ERROR',
                                 'filters': ['require_debug_false'],
                                 'class': 'django.utils.log.AdminEmailHandler'
                                 }
                 },
    'loggers': {'django.request': {'handlers': ['mail_admins'],
                                   'level': 'ERROR',
                                   'propagate': True,
                                   },
                }
    }
