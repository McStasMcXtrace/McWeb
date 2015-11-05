from django.conf.urls import patterns, url, include
# Uncomment the next two lines to enable the admin:
from django.contrib import admin
admin.autodiscover()

urlpatterns = [
    url(r'^admin/', include(admin.site.urls)),
    url(r'^sim_file_upload/$', 'mcupload.views.sim_file_upload' , name='SimulationUpload'),
    url(r'^sim_file_upload/upload_sim_file/$', 'mcupload.views.upload_sim_file', name='SimulationUpload'),
]
