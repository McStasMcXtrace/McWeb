'''
ldap functions using ldifs tooo add/remove users, change password and init the db for mcweb usage.
'''
import os
import subprocess
import re
from mcweb.settings import MCWEB_LDAP_DN, LDAP_PW
from signupper.models import Signup
from django.utils import timezone

class LdapUserEntry:
    ''' record contents of list returned by listusers()'''
    def __init__(self, uid, cn, sn, mail):
        self.uid = uid
        self.cn = cn
        self.sn = sn
        self.mail = mail
    def __str__(self):
        return 'uid: %s\ncn: %s\nsn: %s\nmail: %s\n' % (self.uid, self.cn, self.sn, self.mail)

def listusers(uid=None):
    '''
    List and return all user entries
    
    Can also be used to return a single user entry, and thus to search for a user entry.
    Always returns a list, this will be of length all_users, 1 or 0 in the cases listed above.
    '''
    if not uid:
        cmd = 'ldapsearch -x -b "ou=users,%s"' % MCWEB_LDAP_DN
    else:
        cmd = 'ldapsearch -x -b "ou=users,%s" "(uid=%s)"' % (MCWEB_LDAP_DN, uid)
    
    proc = subprocess.Popen(cmd, 
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            shell=True)
    com = proc.communicate()
    print('%s\n' % cmd)
    #print('std-out: %s' % com[0])
    if com[1] != '': 
        print('std-err: %s' % com[1])
    
    sections = str.split(com[0], 'dn: uid=')
    users = []

    def resafe(pat, string, default):
        m = re.search(pat, string)
        if m:
            return m.group(1).strip()
        else:
            return default

    for section in sections:
        try:
            uid = re.search('(\w+),', section).group(1).strip()
            cn = resafe('cn: ([\w\s]+)\n', section, 'NOCNFOUND')
            sn = resafe('sn: ([\w\s]+)\n', section, 'NOSNFOUND')
            mail = resafe('mail: ([\w\.\@\-0-9]+)\n', section, 'NOMAILFOUND')
            users.append(LdapUserEntry(uid, cn, sn, mail))
        except:
            print('regex error')
    return users

def addsignup(signup):
    '''
    Add signup to ldap proxy. This function is "LDAP already exists"-safe.
    WARNING: returns the "actual" signup object, arg may be deleted.
    '''
    try:
        adduser(MCWEB_LDAP_DN, LDAP_PW, cn=signup.firstname, sn=signup.lastname, uid=signup.username, email=signup.email, pw=signup.password)
        signup.is_in_ldap = True
        signup.save()
    except AlreadyExistsException as e:
        existing_email = listusers(uid=signup.username)[0].mail
        if existing_email == signup.email: 
            # here we assume that this is the same user
            
            # fetch existing signup with the same name
            username_qs = Signup.objects.filter(username=signup.username)
            old_qs = username_qs.filter(is_in_ldap=True)
            if len(old_qs) > 1:
                raise Exception("ldaputils.addsignup: Signup db may need a cleanup.")
            elif len(old_qs) == 1:
                old = old_qs[0]
                # update new entry with ldap info and extra
                signup.fail_str = old.fail_str + ', last created: %s' % old.created.strftime("%Y%m%d")
                ldap_info = listusers(MCWEB_LDAP_DN, signup.username)[0]
                signup.firstname = ldap_info.cn
                signup.lastname = ldap_info.sn
                # only real guess of a password we have is the original
                signup.password = old.password
                signup.notified = old.notified
                signup.is_in_ldap = True
                signup.save()
                # we have to remove an entry to avoid duplicates, and someone else may save Signup since this responsibility is unclear
                old.delete()
            else:
                # if it does not exist:
                signup.is_in_ldap = True
        else:
            signup.fail_str = signup.fail_str + ', ldapadduser: ' + str(e)
            raise Exception("username %s is taken" % signup.username)

class AlreadyExistsException(Exception): pass

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
        print('running: %s' % ' '.join(cmd))
        if stderr != '': 
            print('std-err: %s' % stderr)
            errmsg = stderr.rstrip('\n')
            if "ldap_add: Already exists" in errmsg:
                raise AlreadyExistsException(errmsg)
            else:
                raise Exception(errmsg)
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
        print('running: %s' % ' '.join(cmd))
        if stderr:
            print('std-err: %s' % stderr)
            ps = ''
            for c in cmd:
                ps = '%s %s' % (ps, c)
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
        print('running: %s' % ' '.join(cmd))
        #print('std-out: %s' % stdout)
        
        if stderr:
            print('std-err: %s' % stderr)
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
        print('running: %s' % ' '.join(cmd))
        #print('std-out: %s' % com[0])
        if com[1] != '':
            print('std-err: %s' % com[1])
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
        if com[1] != '':
            print('std-err: %s' % com[1])
    
    finally:
        os.remove('_ou_users.ldif')

def synchronize(signups, dry=False, verbose=False):
    ''' sync signup field is_in_ldap to the ldap db '''
    ldap_uids =  [u.uid for u in listusers()]
    
    subset = [s for s in signups if s.username in ldap_uids]
    disjoint = [s for s in signups if s not in subset]

    if not dry:
        for s in subset: s.is_in_ldap = True
        for s in disjoint: s.is_in_ldap = False
        [s.save() for s in signups]
    else:
        print('')
        print("-- %d ldap uids --" % len(ldap_uids))
        print('')
        if verbose:
            for u in ldap_uids:
                print(u)
        
        print('')
        print('-- %d matches with local --' % len(subset))
        print('')
        if verbose:
            for u in subset:
                print(u)

