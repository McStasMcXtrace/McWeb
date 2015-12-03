"""
url patterns for simrunner
"""
from django.conf.urls import url
from simrunner import views

urlpatterns = [
    url(r'^instrument/(?P<group_name>\w+)/(?P<instr_name>\w+)/?$', views.instrument, name="instrument"),
    url(r'^instrument/(?P<group_name>\w+)/?$', views.instrument, name="instrument"),
    url(r'^startsim/?$', views.instrument_post, name="instrument_post" ),

    url(r'^simrun/(?P<sim_id>\w+)/?$', views.simrun, name="simrun" ),
    url(r'^simrun-log/(?P<sim_id>\w+)/?$', views.simrun_log, name="simrun_log" ),

    url(r'^login/?$', views.login_post, name='login_post'),
    url(r'^logout/?$', views.logout_user, name='logout_user'),
    url(r'^', views.home, name="home" ),
]
