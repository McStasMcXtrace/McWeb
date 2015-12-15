#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
This scripts adds LDAP users. Init the mcldap db before running this script.
NOTE: sudo usage can not be avoided, because LDAP utils require it.
'''
import logging
import argparse
import subprocess
import os
import csv
    
def ldap_adduser(dn, admin_password, cn, sn, uid, pw, uid_number=1001):
    ''' 
    cn: firstname
    sn: lastname
    uid: username
    pw: password
    
    uid_number: a unique number for this user in the LDAP db (must be kept track of)
    '''
    with open ('cn_user.ldif', 'r') as ldif_template:
        cn_user=ldif_template.read()
        ldif_template.close()
    
    cn_user = cn_user.replace('DN', dn)
    cn_user = cn_user.replace('UCN', cn)
    cn_user = cn_user.replace('USN', sn)
    cn_user = cn_user.replace('UID', uid)
    cn_user = cn_user.replace('uID_NUM', str(uid_number))
    cn_user = cn_user.replace('PASSWORD', pw)    
    
    ldif = open('_cn_user.ldif', 'w')
    ldif.write(cn_user)
    ldif.close()
    try:
        process = subprocess.Popen(['sudo', 'ldapadd', '-x', '-w', admin_password, '-D', 'cn=admin,' + dn, '-f', '_cn_user.ldif'])
        process.communicate()
    finally:
        os.remove('_cn_user.ldif')

def get_dn():
    process = subprocess.Popen('sudo slapcat | head -n1 | cut -d" " -f2',
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               shell=True)
    (stdout, stderr) = process.communicate()
    return stdout.rstrip('\n')

def get_new_uid():
    ''' opens uidindexfile, iterates, saves and returns '''
    if not os.path.exists('uidindexfile'):
        with open('uidindexfile', 'w') as uidif:
            uidif.write('10001')
    
    current = -1
    with open('uidindexfile', 'r') as uidif:
        current = uidif.read().strip()
        uidif.close()
    with open('uidindexfile', 'w') as uidif:
        next = int(current) + 1
        uidif.write(str(next))
        uidif.close()
        return next

def main(args):
    ''' assumes a csv format of "firstname;lastname;username;email;password" (email is not supported, used elsewhere) '''
    logging.basicConfig(level=logging.INFO)
    
    dn = get_dn()
    
    debug = True
    if debug:
        dn = 'dc=fysik,dc=dtu,dc=dk'
        
    with open(args.users_csv[0], 'r') as csvfile:
        reader = csv.reader(csvfile, delimiter=';')
        firstline = True
        for row in reader:
            if not firstline:
                nextuid = get_new_uid()
                ldap_adduser(dn, args.password[0], cn=row[0], sn=row[1], uid=row[2], pw=row[4], uid_number=nextuid)
            firstline = False

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('password', nargs='+', help='ldap admin password')
    parser.add_argument('users_csv', nargs='+', help='csv file containing user data')
    args = parser.parse_args()

    main(args)
