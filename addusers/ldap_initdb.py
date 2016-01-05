#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
Init the ldap db for usage by ldap_addusers
'''
import logging
import argparse
import subprocess
import os
from ldap_addusers import get_dn
    
def ldap_initdb(dn, password):
    
    # prepare ldif 1
    with open ("cn_usergroup.ldif", "r") as ldif_template:
        cn_usergroup=ldif_template.read()
        ldif_template.close()
    cn_usergroup = cn_usergroup.replace('DN', dn)
    
    # apply ldif 1
    ldif = open('_cn_usergroup.ldif', 'a')
    ldif.write(cn_usergroup)
    ldif.close()
    try:
        process = subprocess.Popen(['sudo', 'ldapadd', '-x', '-w', password, '-D', 'cn=admin,' + dn, '-f', '_cn_usergroup.ldif'])
        process.communicate()
    finally:
        os.remove('_cn_usergroup.ldif')
    
    # prepare ldif 2
    with open ("ou_users.ldif", "r") as ldif_template:
        ou_users=ldif_template.read()
        ldif_template.close()
    ou_users = ou_users.replace('DN', dn)
    
    # apply ldif 2
    ldif = open('_ou_users.ldif', 'a')
    ldif.write(ou_users)
    ldif.close()
    try:
        process = subprocess.Popen(['sudo', 'ldapadd', '-x', '-w', password, '-D', 'cn=admin,' + dn, '-f', '_ou_users.ldif'])
        process.communicate()
    finally:
        os.remove('_ou_users.ldif')

def main(args):
    logging.basicConfig(level=logging.INFO)
    
    ldap_initdb(get_dn(), args.password[0])

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('password', nargs=1, help='ldap admin password')
    args = parser.parse_args()

    main(args)
