'''
ldap functions using ldifs tooo add/remove users, change password and init the db for mcweb usage.
'''
import os
import subprocess 


def ldap_adduser():
    pass

def ldap_rmuser():
    pass

def ldap_chpassword():
    pass
    
def ldap_initdb(dn, password):
    ''' inits  the ldap db for mcweb user addition '''
    # add cn_usergroup
    cn_usergroup = 'dn: cn=usergroup,%s\ngidNumber: 500\ncn: usergroup\nobjectClass: posixGroup\nobjectClass: top' % dn    
    ldif = open('_cn_usergroup.ldif', 'a')
    ldif.write(cn_usergroup)
    ldif.close()
    try:
        process = subprocess.Popen(['ldapadd', '-x', '-w', password, '-D', 'cn=admin,' + dn, '-f', '_cn_usergroup.ldif'])
        process.communicate()
    finally:
        os.remove('_cn_usergroup.ldif')

    # add ou_users
    ou_users = 'dn: ou=users,%s\nobjectClass: organizationalUnit\nobjectClass: top\nou: users\n' % dn
    ldif = open('_ou_users.ldif', 'a')
    ldif.write(ou_users)
    ldif.close()
    try:
        process = subprocess.Popen(['sudo', 'ldapadd', '-x', '-w', password, '-D', 'cn=admin,' + dn, '-f', '_ou_users.ldif'])
        process.communicate()
    finally:
        os.remove('_ou_users.ldif')
