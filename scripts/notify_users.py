#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
This script is used to send "welcome to e-neutrons.org" emails to users that have presumably 
been added to the ldap db.
'''
import logging
import argparse
from datetime import datetime
import subprocess
import csv
import os

def printtodaystr():
    ''' prints the todaystring '''
    print gettodaystr()
    
def gettodaystr():
    ''' this string is part of the filename of csv files '''
    return datetime.now().strftime("%Y%m%d")

def sendnotify(fullname, username, email, password):
    ''' send an accout-created notification email to specified user '''
    body = '''
Dear %s

You have been added to http://e-neutrons.org.

username: %s
password: %s
    ''' % (fullname, username, password)
    
    try:
        f = open('_body', 'w') 
        f.write(body)
        f.close()
        cmd = 'mailx -s "welcome to mcweb" %s < _body' % email
        #print(cmd)
        subprocess.call(cmd, shell=True)
    finally:
        os.remove('_body')

def main(args):
    logging.basicConfig(level=logging.INFO)
    
    input_filename = args.notify_users_csv[0]
    with open(input_filename, 'r') as csvfile:
        reader = csv.reader(csvfile, delimiter=',')

        for row in reader:
            # filter header line out
            if row[3] != 'email':
                try:
                    sendnotify('%s %s' % (row[0], row[1]), row[2], row[3], row[4])
                    print('notification email sent to %s' % row[3])
                except Exception as e: 
                    print('error sending email to %s (%s)' % (row[3], e.__str__()))
            
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('notify_users_csv', nargs=1, help='csv file containing user data')
    args = parser.parse_args()

    main(args)
