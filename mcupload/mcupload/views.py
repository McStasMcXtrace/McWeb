from django.contrib.auth import authenticate
from django.shortcuts import render

from .forms import SimFileForm
from sim_file_handler import shepherd

NONCE_NAME = 'csrfmiddlewaretoken'

def upload_sim_file(request):
    form = SimFileForm(request.POST, request.FILES)
    user = authenticate(username=form['username'].value(), password=form['password'].value())
    print ""
    if user is None or not user.is_active:
        print "UPLOAD USER COULD NOT AUTHENTICATE"

    if form.is_valid():
        print "UPLOAD FORM WAS VALID"
        print shepherd(request.FILES['sim_file'], form['group'])
    else:
        print "UPLOAD FORM IS INVALID"

    form = SimFileForm()
    return render(request, 'sim_file_upload.html', {form: 'form'})



