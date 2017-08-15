'''
ldap functions using ldifs tooo add/remove users, change password and init the db for mcweb usage.
'''
import os
import subprocess 

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
                                   stderr=subprocess.PIPE,)
        (stdout, stderr) = process.communicate()
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
                                   stderr=subprocess.PIPE,)
        (stdout, stderr) = process.communicate()
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

def find_user(dn, uid):
    '''
    Check LDAP db for entries of uid field equal to input.
    '''

def chpassword(dn, admin_password, uid, current_password, new_password):
    ''' 
    This is a change password function. It only works with the admin password.
    
    uid: username
    current_password: the current password of user, identified with uid
    new_password: the new password
    '''
    chpassword = 'dn: uid=%s,ou=users,%s\nchangetype: modify\ndelete: userpassword\nuserpassword: %s\n-\nadd: userpassword\nuserpassword: %s' % (uid, dn, current_password, new_password)
    
    ldif = open('_chpassword.ldif', 'w')
    ldif.write(chpassword)
    ldif.close()
    try:
        cmd = ['ldapadd', '-x', '-w', admin_password, '-D', 'cn=admin,' + dn, '-f', '_chpassword.ldif']
        process = subprocess.Popen(cmd,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)
        (stdout, stderr) = process.communicate()
        if stderr:
            ps = ''
            for c in cmd: ps = '%s %s' % (ps, c)
            raise Exception(stderr.replace('\n', ''))
    finally:
        os.remove('_chpassword.ldif')

def _chfield(dn, admin_password, uid, value_name, current_value, new_value):
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
        if stderr:
            ps = ''
            for c in cmd:
                ps = '%s %s' % (ps, c)
            raise Exception(stderr.replace('\n', ''))
    finally:
        os.remove('_chvalue.ldif')

def chcn(dn, admin_password, uid, current_cn, new_cn):
    '''
    Chance cn of user.
    '''
    _chfield(dn, admin_password, uid, 'cn', current_value=current_cn, new_value=new_cn)

def chsn(dn, admin_password, uid, current_sn, new_sn):
    '''
    Chance sn of user.
    '''
    _chfield(dn, admin_password, uid, 'sn', current_value=current_sn, new_value=new_sn)

def chemail(dn, admin_password, uid, current_email, new_email):
    '''
    Chance email of user.
    '''
    _chfield(dn, admin_password, uid, 'semail', current_value=current_email, new_value=new_email)

# a very local test:
# _chfield('dc=risoe,dc=dk', 'secret_admin_pass', 'jaga15', 'userpassword', 'hest1', 'hest2')

def initdb(dn, admin_password):
    ''' inits  the ldap db for mcweb user addition '''
    # add cn_usergroup
    cn_usergroup = 'dn: cn=usergroup,%s\ngidNumber: 500\ncn: usergroup\nobjectClass: posixGroup\nobjectClass: top' % dn    
    ldif = open('_cn_usergroup.ldif', 'a')
    ldif.write(cn_usergroup)
    ldif.close()
    try:
        process = subprocess.Popen(['ldapadd', '-x', '-w', admin_password, '-D', 'cn=admin,' + dn, '-f', '_cn_usergroup.ldif'])
        process.communicate()
    finally:
        os.remove('_cn_usergroup.ldif')

    # add ou_users
    ou_users = 'dn: ou=users,%s\nobjectClass: organizationalUnit\nobjectClass: top\nou: users\n' % dn
    ldif = open('_ou_users.ldif', 'a')
    ldif.write(ou_users)
    ldif.close()
    try:
        process = subprocess.Popen(['ldapadd', '-x', '-w', admin_password, '-D', 'cn=admin,' + dn, '-f', '_ou_users.ldif'])
        process.communicate()
    finally:
        os.remove('_ou_users.ldif')
