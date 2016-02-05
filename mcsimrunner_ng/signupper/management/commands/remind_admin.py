'''
Send a command to the admin reminding of new signups.
'''
from django.core.management.base import BaseCommand
import os
import subprocess
from signupper.utils import get_signups
from mcweb.settings import MCWEB_ADMIN_EMAIL

class Command(BaseCommand):
    ''' remind admin comman '''
    help = 'Sends an email reminder to mcweb.settings.MCWEB_ADMIN_EMAIL if there are new signups. '

    def add_arguments(self, parser):
        ''' list arguments for this command '''
        parser.add_argument('adminemail', nargs='*', type=str, help='extra email addresses to which the admin reminder is sent, in addition to the django setting MCWEB_ADMIN_EMAIL')
        
    def handle(self, *args, **options):
        ''' Command impl. '''
        
        admin_email = '''Hello admin,

There are new signups. Please visit <a href="http://sim.e-neutrons.org/login_au">sim.e-neutrons.org/login_au</a> to perform the listed admin tasks.

At the login screen, you must enter the credentials of a django super-user account, as well as the ldap password.

Please note the following:
1) To manage course subscription or other tasks for new or existing users in Moodle, please use the Moodle admin pages at <a href="http://www.e-neutrons.org/moodle/.....">www.e-neutrons.org/moodle/...</a>.
2) If you need to edit the signup instances, use the django admin tool.\n'''
        
        # only send admin email if there are "new" signups
        if len(get_signups()) == 0:
            return
        
        # parse email and addresses, and send admin email, catching errors and clearning up
        try:
            email_addrses = MCWEB_ADMIN_EMAIL
            for address in options['adminemail']:
                email_addrses = '%s %s' % (email_addrses, address)
            cmd = 'mailx -s "mcweb admin: new signups" %s < _admin_email' % email_addrses
            print(cmd)
            
            f = open('_admin_email', 'w') 
            f.write(admin_email)
            f.close()
            
            retcode = subprocess.call(cmd, shell=True)
            if retcode != 0:
                raise Exception('retcode: %s' % retcode) 
        
        except Exception as e:
            print('%s' % (e.__str__()))
        
        finally:
            os.remove('_admin_email')
