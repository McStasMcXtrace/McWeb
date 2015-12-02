from django.conf.urls import patterns, url, include
import views

urlpatterns = [
    url(r'^upload_sim_file/$', 'mcupload.views.upload_sim_file', name='SimulationUpload'),
]
