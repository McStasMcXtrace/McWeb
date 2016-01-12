"""
url patterns for signupper
"""
from django.conf.urls import url
from signupper import views

urlpatterns = [
    url(r'^signup/?$', views.signup, name="signup" ),
    url(r'^signup-get/?', views.signup_get, name="signup_get" ),
    
    url(r'^changepassword/?$', views.chpassword, name="chpassword" ),
    
    url(r'^thanks/?$', views.thanks, name="thanks" ),
]
