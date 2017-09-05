"""
url patterns for signupper
"""
from django.conf.urls import url
from signupper import views

urlpatterns = [
    # demo site
    url(r'^contact/?$', views.contact, name="contact"),
    url(r'^signup/?$', views.signup, name="contact"),
    url(r'^signup-get/?$', views.signup_get, name="contact"),
    url(r'^thanks/?$', views.thanks, name="contact"),
    url(r'^howmany/?$', views.num_signups, name="num_signups"),
    
    # signup and course management module
    url(r'^superlogin/?$', views.superlogin, name="superlogin" ),
    url(r'^manage/?$', views.manage, name="manage" ),
    url(r'^manage/(?P<menu>[\w]+)/(?P<post>[\w]+)$', views.manage, name="manage" ),
    url(r'^manage/(?P<menu>[\w]+)/?$', views.manage, name="manage" ),
]
