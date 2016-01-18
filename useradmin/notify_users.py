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
import posix

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

You have been added to the http://e-neutrons.org e-Learning system.

username: %s
password: %s

To change your password, please visit http://www.e-neutrons.org/ssp/

Best,

The e-neutrons.org admin team
    ''' % (fullname, username, password)
    
    try:
        f = open('_body', 'w') 
        f.write(body)
        f.close()
        cmd = 'mailx -s "welcome to mcweb" %s < _body' % email
        #print(cmd)
        retcode = subprocess.call(cmd, shell=True)
        if retcode != 0:
            raise Exception('retcode: %s' % retcode) 
    finally:
        os.remove('_body')

def cols_to_line(cols, delimiter = ','):
    ''' makes a list of strings into a single ;-separated string with a trailing linebreak '''
    line = ""
    for c in cols:
        line = "%s%s%s" % (line, c, delimiter)
    return line.rstrip("%s" % delimiter) + "\n"

def main(args):
    ''' script-specific functionality: with input_file, read lines, send emails and record this in "description" column (index 5) '''
    logging.basicConfig(level=logging.INFO)
    
    # used to record org and modified lines to be written to csv below
    text = ''

    input_filename = args.notify_users_csv[0]
    
    # writable-check to avoid sending emails without being able to record this in the csv
    if not os.access(input_filename, posix.W_OK):
        print('refuse, exiting: file %s must be writable' % input_filename)
        exit()
    
    # send notification emails and record all (modified) lines
    with open(input_filename, 'r') as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        
        for row in reader:
            # safety for empty or nonsensical lines - skip 
            if len(row) < 6:
                continue;
            
            # filter header line out
            if row[3] != 'email':
                if row[5] == '0':
                    try:
                        sendnotify('%s %s' % (row[0], row[1]), row[2], row[3], row[4])
                        row[5] = 'user notified by email on %s' % gettodaystr()
                        
                        print('notification email sent to %s' % row[3])
                    except Exception as e: 
                        print('error sending email to %s (%s)' % (row[3], e.__str__()))
            
            # record each (un/modified) row as a line of text
            text = text + cols_to_line(row)

        csvfile.close()
    
    # write modified csv to input file
    with open(input_filename, 'w') as csvfile:
        csvfile.write(text)
        csvfile.close()
        
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('notify_users_csv', nargs=1, help='csv file containing user data')
    args = parser.parse_args()

    main(args)
