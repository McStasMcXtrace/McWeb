"""
url patterns for uploader
"""
from django.conf.urls import url
from uploader import views

urlpatterns = [
    url(r'^upload/?$', views.upload, name="upload" ),
]
