"""
url patterns for signupper
"""
from django.conf.urls import url
from signupper import views

urlpatterns = [
    url(r'^signup/?$', views.signup, name="signup" ),
    url(r'^signup-get/?', views.signup_get, name="signup_get" ),
    url(r'^thanks/?$', views.thanks, name="thanks" ),
    
    url(r'^changepassword/?$', views.chpassword, name="chpassword" ),
    
    url(r'^login_au/?$', views.login_au, name="login_au" ),
    url(r'^userlist_au/?$', views.userlist_au, name="userlist_au" ),
    url(r'^userdetail_au/?$', views.userdetail_au, name="userdetail_au" ),
    
    url(r'^signup_au/?$', views.signup_au, name="signup_au" ),
    url(r'^signup_au-get/?$', views.signup_au_get, name="signup_au_get" ),
]
