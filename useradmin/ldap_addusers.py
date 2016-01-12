#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
This scripts adds users from a mcsimrunner_ng.signupper moodle-compatible csv to ldap. 
NOTE: run ldap_initdb before adding users with this script.
'''
import logging
import argparse
import subprocess
import os
import re
from os.path import join, dirname, abspath, exists
import csv
from datetime import datetime

class LDAPuserException(Exception):
    ''' signals a failed application of an add-user ldif '''
    pass
    
def ldap_adduser(dn, admin_password, cn, sn, uid, mail, pw, uid_number=1001):
    ''' 
    cn: firstname
    sn: lastname
    uid: username
    pw: password
    
    uid_number: a unique number for this user in the LDAP db
    '''
    with open('ldifs/uid_user.ldif', 'r') as ldif_template:
        uid_user=ldif_template.read()
        ldif_template.close()
    
    uid_user = uid_user.replace('DN', dn)
    uid_user = uid_user.replace('UCN', cn)
    uid_user = uid_user.replace('USN', sn)
    uid_user = uid_user.replace('MAIL', mail)
    uid_user = uid_user.replace('UID', uid)
    uid_user = uid_user.replace('uID_NUM', str(uid_number))
    uid_user = uid_user.replace('PASSWORD', pw)    
    
    ldif = open('_uid_user.ldif', 'w')
    ldif.write(uid_user)
    ldif.close()
    try:
        cmd = ['ldapadd', '-x', '-w', admin_password, '-D', 'cn=admin,' + dn, '-f', '_uid_user.ldif']
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
        os.remove('_uid_user.ldif')

def get_dn():
    ''' get the LDAP dn silently via the command line '''
    process = subprocess.Popen('sudo slapcat | head -n1 | cut -d" " -f2',
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               shell=True)
    (stdout, stderr) = process.communicate()
    return stdout.rstrip('\n')

def get_new_uid():
    ''' opens _uidindexfile, iterates, saves and returns '''
    if not exists('_uidindexfile'):
        with open('_uidindexfile', 'w') as uidif:
            uidif.write('10001')
    
    with open('_uidindexfile', 'r') as uidif:
        current = uidif.read().strip()
        uidif.close()
    with open('_uidindexfile', 'w') as uidif:
        nextuid = int(current) + 1
        uidif.write(str(nextuid))
        uidif.close()
        return nextuid

def list_to_delimited_str(row_lst, delimiter=','):
    ''' takes a list of str values and outputs a csv-row '''
    return reduce(lambda cell_a, cell_b: '%s%s%s' % (cell_a, delimiter, cell_b), row_lst)

def addusers(dn, newsignups_filename):
    ''' add users given a dn and a moodle-compatible csv-file '''
    users_notadded = []
    users_added = []
    
    # read and process input rows
    with open(newsignups_filename, 'r') as csvfile:
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
                    ldap_adduser(dn, args.password[0], cn=row[0], sn=row[1], uid=row[2], mail=row[3], pw=row[4], uid_number=nextuid)
                    users_added.append(list_to_delimited_str(row))
                    print('uid "%s" added to archive' % row[2])
                except LDAPuserException as e:
                    users_notadded.append(list_to_delimited_str(row))
                    print('uid "%s" not added (%s)' % (row[2], e.message))
    
    return users_added, users_notadded

def main(args):
    ''' assumes a csv format of "firstname,lastname,username,email,password,auth,course1,course2,..." 
        NOTE: handles the specifics of this script - general functionality (ldap access etc.) should be 
        encapsulated in various functions.
    '''
    logging.basicConfig(level=logging.INFO)
    
    input_filename = args.users_csv[0]
    # sanity check for input csv
    if re.match('added', input_filename):
        print('WARNING: input csv must not start with "added"')
        exit()
    
    # get dn via command line (prompts the user for sudo password)
    dn = get_dn()
    
    # add users to ldap
    users_added, users_notadded = addusers(dn, input_filename)
    
    # write/update archive file
    basedir = abspath(dirname(__file__))
    added_name = join(basedir, datetime.now().strftime("added_%Y%m%d.csv"))
    if len(users_added) > 1:
        
        # don't write the header if file exists (first index of users_added list)
        if exists(added_name):
            users_added.remove(users_added[0])
        
        # open todays archive file in append mode
        with open(added_name, 'a') as csvfile_archive:
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
        print("removing org file and creating a copy of %s in moodledata ..." %added_name)
        subprocess.call('./cptomoodledata.sh %s' % added_name, shell=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('password', nargs=1, help='ldap admin password')
    parser.add_argument('users_csv', nargs=1, help='csv file containing user data')
    args = parser.parse_args()

    main(args)
