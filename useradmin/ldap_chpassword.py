#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
This scripts changes an existing user's password in ldap. Both ldap admin password, user uid, current user password and new password must be supplied.
'''
import logging
import argparse
import subprocess
import os
import sys
from ldap_addusers import get_dn

class LDAPuserException(Exception):
    ''' signals a failed application of an add-user ldif '''
    pass
    
def ldap_chpassword(dn, admin_password, uid, current_password, new_password):
    ''' 
    uid: username
    current_password: the current password of user identified with uid
    new_password: the new password will be set to this 
    '''
    with open('ldifs/chpassword.ldif', 'r') as ldif_template:
        chpassword=ldif_template.read()
        ldif_template.close()
    
    chpassword = chpassword.replace('DN', dn)
    chpassword = chpassword.replace('UID', uid)
    chpassword = chpassword.replace('CURRENTPW', current_password)
    chpassword = chpassword.replace('NEWPW', new_password)
    
    ldif = open('_chpassword.ldif', 'w')
    ldif.write(chpassword)
    ldif.close()
    try:
        cmd = ['sudo', 'ldapadd', '-x', '-w', admin_password, '-D', 'cn=admin,' + dn, '-f', '_chpassword.ldif']
        process = subprocess.Popen(cmd,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE,)
        (stdout, stderr) = process.communicate()
        if stderr:
            ps = ''
            for c in cmd: ps = '%s %s' % (ps, c)
            raise LDAPuserException(stderr.replace('\n', ''))
    finally:
        os.remove('_chpassword.ldif')

def chpassword(admin_pw, uid, current, new):
    ''' gets the dn and calls ldap_chpassword (generic python call level) '''
    dn = get_dn()
    ldap_chpassword(dn, admin_pw, uid=uid, current_password=current, new_password=new)

def main(args):
    ''' assuming that all required args exist (direct script usage level) '''
    logging.basicConfig(level=logging.INFO)
    
    try:
        chpassword(args.password[0], args.uid, args.currentpw, args.newpw)
        print('uid "%s" password changed success' % args.uid)
    except LDAPuserException as e:
        print('uid "%s" password change fail (%s)' % (args.uid, e.message))
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('password', nargs=1, help='ldap admin password')
    parser.add_argument('-u', '-useruid', action='store', dest='uid', help='uid of single user to be removed from dlap')
    parser.add_argument('-c', '-current', action='store', dest='currentpw', help='current password')
    parser.add_argument('-n', '-new', action='store', dest='newpw', help='new password')
    args = parser.parse_args()
    
    if not args.uid or not args.currentpw or not args.newpw:
        parser.print_help()
        sys.exit(2)
    
    main(args)
