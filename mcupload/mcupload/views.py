# django imports
from django.contrib.auth import authenticate
from django.shortcuts import render_to_response
from django.template import RequestContext
# from django.core.files.uploadedfile import SimpleUploadedFile
# app imports
from .forms import SimFileForm
from common import only_safe, only_unsafe, templated
from sim_file_handler import shepherd, group_dirs
# consts
NONCE_NAME = 'csrfmiddlewaretoken'

def upload_sim_file(request):
    form = SimFileForm(request.POST, request.FILES)
    user = authenticate(username=form['username'], password=form['password'])
    print ""
    for line in form:
        print str(line)+"\n"
    print ""
    if user is None or not user.is_active:
        print "USER COULD NOT AUTHENTICATE"
        # display user could not auth on next page
    if form.is_valid(): # CHANGE TO elif WHEN AUTHENTICATION IS SORTED
        print "FORM WAS VALID"
        print shepherd(request.FILES['sim_file'], form['group'])
        # put shepherd return on the next page
    else:
        print "FORM IS INVALID"
        # invalid form message
    form = SimFileForm()
    return render_to_response('sim_file_upload.html', {form: 'form'}, context_instance=RequestContext(request))


