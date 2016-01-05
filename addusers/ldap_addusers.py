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
from os.path import join, dirname, abspath, exists
import csv
from datetime import datetime


class LDAPuserException(Exception):
    ''' signals a failed application of an add-user ldif '''
    pass
    
def ldap_adduser(dn, admin_password, cn, sn, uid, pw, uid_number=1001):
    ''' 
    cn: firstname
    sn: lastname
    uid: username
    pw: password
    
    uid_number: a unique number for this user in the LDAP db
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
        cmd = ['sudo', 'ldapadd', '-x', '-w', admin_password, '-D', 'cn=admin,' + dn, '-f', '_cn_user.ldif']
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
        os.remove('_cn_user.ldif')

def get_dn():
    ''' get the LDAP dn silently via the command line '''
    process = subprocess.Popen('sudo slapcat | head -n1 | cut -d" " -f2',
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               shell=True)
    (stdout, stderr) = process.communicate()
    return stdout.rstrip('\n')

def get_new_uid():
    ''' opens uidindexfile, iterates, saves and returns '''
    if not exists('uidindexfile'):
        with open('uidindexfile', 'w') as uidif:
            uidif.write('10001')
    
    with open('uidindexfile', 'r') as uidif:
        current = uidif.read().strip()
        uidif.close()
    with open('uidindexfile', 'w') as uidif:
        nextuid = int(current) + 1
        uidif.write(str(nextuid))
        uidif.close()
        return nextuid

def list_to_delimited_str(row_lst, delimiter=','):
    ''' takes a list of str values and outputs a csv-row '''
    return reduce(lambda cell_a, cell_b: '%s%s%s' % (cell_a, delimiter, cell_b), row_lst)

def main(args):
    ''' assumes a csv format of "firstname,lastname,username,email,password,auth,course1,course2,..." '''
    logging.basicConfig(level=logging.INFO)
    
    # get dn via command line (prompts the user for sudo password)
    dn = get_dn()
    basedir = abspath(dirname(__file__))
    
    # debug mode (eclipse cannot prompt)
    debug = False
    if debug:
        dn = 'dc=fysik,dc=dtu,dc=dk'
    
    # house keeping     
    users_notadded = []
    users_added = []
    input_filename = args.users_csv[0]
    
    # read and process input rows
    with open(input_filename, 'r') as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        firstline = True # header line housekeeping
        
        # iterate to add users
        for row in reader:
            # copy header lines to output lists
            if firstline:
                users_added.append(list_to_delimited_str(row))
                users_notadded.append(list_to_delimited_str(row))
                firstline = False
            else:
                try:
                    nextuid = get_new_uid()
                    ldap_adduser(dn, args.password[0], cn=row[0], sn=row[1], uid=row[2], pw=row[4], uid_number=nextuid)
                    users_added.append(list_to_delimited_str(row))
                    print('uid "%s" added to archive' % row[2])
                except LDAPuserException as e:
                    users_notadded.append(list_to_delimited_str(row))
                    print('uid "%s" not added (%s)' % (row[2], e.message))

    # write/update archive file
    if len(users_added) > 1:
        
        # don't write the header if file exists (first index of users_added list)
        name = join(basedir, datetime.now().strftime("added_%Y%m%d.csv"))
        if exists(name):
            users_added.remove(users_added[0])
        
        # open todays archive file in append mode
        with open(name, 'a') as csvfile_archive:
            try:
                # write lines
                text = list_to_delimited_str(users_added, delimiter='\n') + '\n'
                csvfile_archive.write(text)
            finally:
                csvfile_archive.close()
        
    elif len(users_notadded) > 1:
        print('no users were added')
        
    # overwrite input file with lines NOT added (note: first line is the header)
    if len(users_notadded) > 1:
        with open(input_filename, 'w') as csvfile:
            try:
                text = list_to_delimited_str(users_notadded, delimiter='\n') + '\n'
                csvfile.write(text)
            finally:
                csvfile.close()
    
    else:
        if len(users_added) > 1:
            print('all users were added')
        os.remove(input_filename)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('password', nargs='+', help='ldap admin password')
    parser.add_argument('users_csv', nargs='+', help='csv file containing user data')
    args = parser.parse_args()

    main(args)
