#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
Init the mcldap db by running this script. Then users may be added.
'''
import logging
import argparse
import subprocess
import os
import csv
    
def ldap_adduser(dn, admin_password, uid, user_password='hest', uid_number=1001):
    with open ("cn_user.ldif", "r") as ldif_template:
        cn_user=ldif_template.read()
        ldif_template.close()
    
    cn_user = cn_user.replace('DN', dn)
    cn_user = cn_user.replace('UID', uid)
    cn_user = cn_user.replace('uID_NUM', str(uid_number))
    cn_user = cn_user.replace('PASSWORD', user_password)    
    
    ldif = open('_cn_user.ldif', 'w')
    ldif.write(cn_user)
    ldif.close()
    try:
        process = subprocess.Popen(['sudo', 'ldapadd', '-x', '-w', admin_password, '-D', 'cn=admin,dc=fysik,dc=dtu,dc=dk', '-f', '_cn_user.ldif'])
        process.communicate()
    finally:
        os.remove('_cn_user.ldif')

def main(args):
    logging.basicConfig(level=logging.INFO)
    
    with open(args.users_csv[0], 'r') as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        for row in reader:
            ldap_adduser(args.dn[0], args.password[0], row[0], row[1])

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('dn', nargs='+', help='run slapcat to identify (e.g. dc=fysik,dc=dtu,dc=dk)')
    parser.add_argument('password', nargs='+', help='ldap admin password')
    parser.add_argument('users_csv', nargs='+', help='csv file containing user data')
    args = parser.parse_args()

    main(args)
