import os
from django.shortcuts import render
from mcweb.settings import BASE_DIR, FILE_UPLOAD_PW

def group_dirs():
    ''' lists base level file directories in sim/ '''
    for _, dirs, _ in os.walk('sim/'):
        return dirs

def shepherd(file_obj, g):
    ''' saves file data in file_obj to the folder associated with group name g '''
    file_path = "%s/%s"%(os.path.join(BASE_DIR, 'sim', g), file_obj.name)
    with open(file_path, 'wb+') as out_file:
        for chunk in file_obj.chunks():
            out_file.write(chunk)
    return "saved to: %s" % file_path

def upload(request):
    ''' handles upload post and injects another form '''
    form = request.POST
    if not form:
        return render(request, 'uploader.html', {'groupdirs': group_dirs(), 'password': '', 'message': ''})
    
    # check password
    pw = form['password']
    if pw == '':
        return render(request, 'uploader.html', {'groupdirs': group_dirs(), 'password': '', 'message': ''})
    
    if pw != FILE_UPLOAD_PW:
        return render(request, 'uploader.html', {'groupdirs': group_dirs(), 'password': '', 'message': 'Please enter the correct password'})
    
    # handle form
    g = form['groupdir']
    
    # mkdir if required (and set g to new group_dir)...
    d = form['new_group']
    if d != '':
        os.mkdir('sim/%s' % d)
        g = d
        
    # get file
    message = ''
    if len(request.FILES) > 0:
        f = request.FILES['up_file']
        shepherd(f, g)
        message = 'File %s uploaded to %s.' % (f.name, g)
    
    # return render based on current state
    return render(request, 'uploader.html', {'groupdirs': group_dirs(), 'password': pw, 'message': message})
