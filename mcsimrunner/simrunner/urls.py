"""
url patterns for simrunner
"""
from django.conf.urls import url
from simrunner import views

urlpatterns = [
    url(r'^instrument/(?P<group_name>[\w-]+)/(?P<instr_name>[\w-]+)/?$', views.instrument, name="instrument"),
    url(r'^instrument/(?P<group_name>[\w-]+)/?$', views.instrument, name="instrument"),
    url(r'^instrument-menu/(?P<group_name>[\w-]+)/(?P<instr_name>[\w-]+)/?$', views.instrument_menu, name="instrument_menu"),
    url(r'^instrument-menu/(?P<group_name>[\w-]+)/?$', views.instrument_menu, name="instrument_menu"),
    
    url(r'^startsim/?$', views.instrument_post, name="instrument_post" ),

    url(r'^simrun/(?P<sim_id>[\w-]+)/?$', views.simrun, name="simrun"),
    
    url(r'^recent/?$', views.recent, name="recent"),

    url(r'^login-post/?$', views.login_post, name='login_post'),
    url(r'^logout/?$', views.logout_user, name='logout_user'),
    
    url(r'^loginstatus/?$', views.login_status, name='login_status'),
    url(r'^loginslim-post/?$', views.login_slim_post, name='login_slim_post'),
    url(r'^logoutslim/?$', views.logout_slim, name='logout_slim'),
    
    url(r'^', views.home, name="home" ),
]
