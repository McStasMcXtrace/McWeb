import os
from django.shortcuts import render
from mcweb.settings import BASE_DIR, FILE_UPLOAD_PW

def group_dirs():
    ''' lists base level file directories in sim/ '''
    for _, dirs, _ in os.walk('sim/'):
        # stackoverflow-ish alphanum sort 
        dirs = sorted(dirs, key=lambda item: (int(item.partition(' ')[0])
                               if item[0].isdigit() else float('inf'), item))
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
    groupdirs = group_dirs()
    message = ''
    
    # try to get form
    form = request.POST
    if not form:
        return render(request, 'uploader.html', {'groupdirs': groupdirs, 'password': '', 'message': ''})
    
    # check password
    pw = form['password']
    if pw == '':
        return render(request, 'uploader.html', {'groupdirs': groupdirs, 'password': '', 'message': ''})
    if pw != FILE_UPLOAD_PW:
        return render(request, 'uploader.html', {'groupdirs': groupdirs, 'password': '', 'message': 'Please enter the correct password'})
    
    # handle form
    g = form['groupdir']
    
    # mkdir if required (and set g to new group_dir)...
    d = form['new_group']
    if d != '':
        os.mkdir('sim/%s' % d)
        message = 'Group dir %s was created.' % d
        groupdirs = group_dirs()
        g = d
    elif g == "select_a_group":
        return render(request, 'uploader.html', {'groupdirs': groupdirs, 'password': pw, 'message': 'Please select or create a group dir.'})
    
    # get file
    if len(request.FILES) > 0:
        f = request.FILES['up_file']
        shepherd(f, g)
        message = 'File %s uploaded to %s.' % (f.name, g)
    else:
        return render(request, 'uploader.html', {'groupdirs': groupdirs, 'password': pw, 'message': '%s Please select a file for upload.' % message})
    
    # return render based on current state
    return render(request, 'uploader.html', {'groupdirs': groupdirs, 'password': pw, 'message': message})
