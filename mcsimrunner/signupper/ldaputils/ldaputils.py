'''
ldap functions using ldifs tooo add/remove users, change password and init the db for mcweb usage.
'''
import os
import subprocess
import re

class LdapUserEntry:
    ''' record contents of list returned by listusers()'''
    def __init__(self, uid, cn, sn, mail):
        self.uid = uid
        self.cn = cn
        self.sn = sn
        self.mail = mail
    def __str__(self):
        return 'uid: %s\ncn: %s\nsn: %s\nmail: %s\n' % (self.uid, self.cn, self.sn, self.mail)

def listusers(dn, uid=None):
    '''
    List and return all user entries
    
    Can also be used to return a single user entry, and thus to search for a user entry.
    Always returns a list, this will be of length all_users, 1 or 0 in the cases listed above.
    '''
    if not uid:
        cmd = 'ldapsearch -x -b "ou=users,%s"' % dn
    else:
        cmd = 'ldapsearch -x -b "ou=users,%s" "(uid=%s)"' % (dn, uid)
    
    proc = subprocess.Popen(cmd, 
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            shell=True)
    com = proc.communicate()
    print('%s\n' % cmd)
    #print('std-out: %s' % com[0])
    #print('std-err: %s' % com[1])
    
    sections = str.split(com[0], 'dn: uid=')
    users = []
    
    for section in sections:
        try:
            uid = re.search('(\w+),', section).group(1).strip()
            cn = re.search('cn: ([\w\s]+)\n', section).group(1).strip()
            sn = re.search('sn: ([\w\s]+)\n', section).group(1).strip()
            mail = re.search('mail: ([\w\.\@\-0-9]+)\n', section).group(1).strip()
            
            users.append(LdapUserEntry(uid, cn, sn, mail))
        except:
            continue
    
    return users

def adduser(dn, admin_password, cn, sn, uid, email, pw):
    ''' 
    Add a user to LDAP using the command-line tool ldapadd.
    
    cn: firstname
    sn: lastname
    uid: username
    email: email
    pw: user password
    '''
    def get_new_uid():
        ''' opens _ldap_uidindexfile, iterates, saves and returns '''
        if not os.path.exists('_uidindexfile'):
            with open('_ldap_uidindexfile', 'w') as uidif:
                uidif.write('10001')
                uidif.close()
        
        with open('_ldap_uidindexfile', 'r') as uidif:
            current = uidif.read().strip()
            uidif.close()
        with open('_ldap_uidindexfile', 'w') as uidif:
            nextuid = int(current) + 1
            uidif.write(str(nextuid))
            uidif.close()
            return nextuid    
    
    uid_number = get_new_uid()    
    
    uid_user = 'dn: uid=%s,ou=users,%s\nobjectClass: top\nobjectClass: inetOrgPerson\nobjectClass: posixAccount\ncn: %s\nsn: %s\nmail: %s\nuid: %s\nuidNumber: %s\ngidNumber: 500\nhomeDirectory: /home/users/%s\nloginShell: /bin/sh\nuserPassword: %s' %  (uid, dn, cn, sn, email, uid, str(uid_number), uid, pw)
    
    ldif = open('_uid_user.ldif', 'w')
    ldif.write(uid_user)
    ldif.close()
    try:
        cmd = ['ldapadd', '-x', '-w', admin_password, '-D', 'cn=admin,' + dn, '-f', '_uid_user.ldif']
        process = subprocess.Popen(cmd,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)
        (stdout, stderr) = process.communicate()
        print('running: %s' % cmd)
        #print('std-out: %s' % stdout)
        #print('std-err: %s' % stderr)
        
        if stderr:
            ps = ''
            for c in cmd: ps = '%s %s' % (ps, c)
            debug = False
            if debug:
                print('"%s" returned an error:' % ps.strip())
                print(stderr.rstrip('\n'))
            raise Exception(stderr.rstrip('\n'))
    finally:
        os.remove('_uid_user.ldif')
    
def rmuser(dn, admin_password, uid):
    ''' 
    Removes a user given by uid from the mcweb-configured LDAP database. 
    
    uid: username
    '''
    rmuser = 'dn: uid=%s,ou=users,%s\nchangetype: delete' % (uid, dn)
    
    ldif = open('_rmuser.ldif', 'w')
    ldif.write(rmuser)
    ldif.close()
    try:
        cmd = ['ldapadd', '-x', '-w', admin_password, '-D', 'cn=admin,' + dn, '-f', '_rmuser.ldif']
        process = subprocess.Popen(cmd,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)
        (stdout, stderr) = process.communicate()
        print('running: %s' % cmd)
        #print('std-out: %s' % stdout)
        #print('std-err: %s' % stderr)
        
        if stderr:
            ps = ''
            for c in cmd: ps = '%s %s' % (ps, c)
            debug = False
            if debug:
                print('"%s" returned an error:' % ps.strip())
                print(stderr.rstrip('\n'))
            raise Exception(stderr.rstrip('\n'))
    finally:
        os.remove('_rmuser.ldif')

def chfield(dn, admin_password, uid, value_name, current_value, new_value):
    '''
    Change a user field if it exists.
    '''
    chfield = 'dn: uid=%s,ou=users,%s\nchangetype: modify\ndelete: %s\n%s: %s\n-\nadd: %s\n%s: %s' % (uid, dn, value_name, value_name, current_value, value_name, value_name, new_value)
    
    ldif = open('_chvalue.ldif', 'w')
    ldif.write(chfield)
    ldif.close()
    try:
        cmd = ['ldapadd', '-x', '-w', admin_password, '-D', 'cn=admin,' + dn, '-f', '_chvalue.ldif']
        process = subprocess.Popen(cmd,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)
        (stdout, stderr) = process.communicate()
        print('running: %s' % cmd)
        #print('std-out: %s' % stdout)
        #print('std-err: %s' % stderr)
        
        if stderr:
            ps = ''
            for c in cmd:
                ps = '%s %s' % (ps, c)
            raise Exception(stderr.replace('\n', ''))
    finally:
        os.remove('_chvalue.ldif')

def initdb(dn, admin_password):
    ''' inits  the ldap db for mcweb user addition '''
    # add cn_usergroup
    cn_usergroup = 'dn: cn=usergroup,%s\ngidNumber: 500\ncn: usergroup\nobjectClass: posixGroup\nobjectClass: top' % dn    
    ldif = open('_cn_usergroup.ldif', 'a')
    ldif.write(cn_usergroup)
    ldif.close()
    try:
        cmd = ['ldapadd', '-x', '-w', admin_password, '-D', 'cn=admin,' + dn, '-f', '_cn_usergroup.ldif']
        process = subprocess.Popen(cmd,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)
        com = process.communicate()
        print('running: %s' % cmd)
        #print('std-out: %s' % com[0])
        #print('std-err: %s' % com[1])
    finally:
        os.remove('_cn_usergroup.ldif')

    # add ou_users
    ou_users = 'dn: ou=users,%s\nobjectClass: organizationalUnit\nobjectClass: top\nou: users\n' % dn
    ldif = open('_ou_users.ldif', 'a')
    ldif.write(ou_users)
    ldif.close()
    try:
        cmd = ['ldapadd', '-x', '-w', admin_password, '-D', 'cn=admin,' + dn, '-f', '_ou_users.ldif']
        process = subprocess.Popen(cmd,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)
        com = process.communicate()
        print('running: %s' % cmd)
        #print('std-out: %s' % com[0])
        #print('std-err: %s' % com[1])

    finally:
        os.remove('_ou_users.ldif')

