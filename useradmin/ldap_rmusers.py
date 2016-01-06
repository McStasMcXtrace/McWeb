#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
This scripts removes users from ldap, assuming a csv with the third column being the uid's, 
one line pr. user to be removed. No file-processing is done.
NOTE: run ldap_initdb before adding users with this script.
'''
import logging
import argparse
import subprocess
import os
import sys
import csv
from ldap_addusers import get_dn

class LDAPuserException(Exception):
    ''' signals a failed application of an add-user ldif '''
    pass
    
def ldap_rmuser(dn, admin_password, uid):
    ''' 
    uid: username
    '''
    with open('ldifs/rmuser.ldif', 'r') as ldif_template:
        rmuser=ldif_template.read()
        ldif_template.close()
    
    rmuser = rmuser.replace('DN', dn)
    rmuser = rmuser.replace('UID', uid)
    
    ldif = open('_rmuser.ldif', 'w')
    ldif.write(rmuser)
    ldif.close()
    try:
        cmd = ['sudo', 'ldapadd', '-x', '-w', admin_password, '-D', 'cn=admin,' + dn, '-f', '_rmuser.ldif']
        process = subprocess.Popen(cmd,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE,)
        (stdout, stderr) = process.communicate()
        if stderr:
            ps = ''
            for c in cmd: ps = '%s %s' % (ps, c)
            debug = False
            if debug:
                print('"%s" returned an error:' % ps.strip())
                print(stderr.rstrip('\n'))
            raise LDAPuserException(stderr.rstrip('\n'))
    finally:
        os.remove('_rmuser.ldif')

def main(args):
    ''' assumes a csv of lines with the third column being the uid's '''
    logging.basicConfig(level=logging.INFO)
    
    # get dn via command line (prompts the user for sudo password)
    dn = get_dn()
    
    # parse csv file
    if args.rm_users_csv:
        input_filename = args.rm_users_csv
    
        # read and process input rows
        with open(input_filename, 'r') as csvfile:
            reader = csv.reader(csvfile, delimiter=',')
            
            # iterate to add users
            for row in reader:
                # skip header line (keeping flexible)
                if row[2] == 'username':
                    continue
                # remove user
                try:
                    ldap_rmuser(dn, args.password[0], uid=row[2])
                    print('uid "%s" removed:' % row[2])
                except LDAPuserException as e:
                    print('uid "%s" not removed (%s):' % (row[2], e.message))
        
    # parse single uid input
    if args.rm_user_uid:
        try:
            uid = args.rm_user_uid
            ldap_rmuser(dn, args.password[0], uid=uid)
            print('uid "%s" removed' % uid)
        except LDAPuserException as e:
            print('uid "%s" not removed (%s)' % (uid, e.message))
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('password', nargs=1, help='ldap admin password')
    parser.add_argument('-f', action='store', dest='rm_users_csv', help='csv file containing users to be removed from dlap')
    parser.add_argument('-u', action='store', dest='rm_user_uid', help='uid of single user to be removed from dlap')
    args = parser.parse_args()
    
    if not args.rm_user_uid and not args.rm_users_csv:
        parser.print_help()
        sys.exit(2)
    
    main(args)
