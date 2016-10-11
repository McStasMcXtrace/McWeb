"""
url patterns for signupper
"""
from django.conf.urls import url
from signupper import views

urlpatterns = [
    
    url(r'^changepassword/?$', views.chpassword, name="chpassword" ),

    # signup form
    url(r'^signup/?$', views.signup_au, name="signup_au" ),
    url(r'^signup_au-get/?$', views.signup_au_get, name="signup_au_get" ),
    url(r'^thanks/?$', views.thanks, name="thanks" ),

    # signup management
    url(r'^login_au/?$', views.login_au, name="login_au" ),
    url(r'^userlist_au/?$', views.userlist_au, name="userlist_au" ),
    url(r'^userlist_au/(?P<listtype>[\w-]+)/?$', views.userlist_au, name="userlist_au" ),
    url(r'^userlist_au-post/?$', views.userlist_au_post, name="userlist_au_post" ),
    url(r'^userlist_au_action/(?P<action>[\w-]+)/(?P<signup_id>[\w-]+)/?$', views.userlist_au_action, name="userlist_au_action" ),
    url(r'^upload_au_post/?$', views.upload_au_post, name="upload_au_post" ),
    url(r'^userdetail_au/(?P<id>[\w-]+)/?$', views.userdetail_au, name="userdetail_au" ),
    
    url(r'^coursemanage/templates?$', views.courseman_templates, name="courseman_templates" ),
    url(r'^coursemanage/templates-post?$', views.courseman_templates_post, name="courseman_templates_post" ),
    url(r'^coursemanage/courses?$', views.courseman_courses, name="courseman_courses" ),
    url(r'^coursemanage/courses-post?$', views.courseman_courses_post, name="courseman_courses_post" ),
    url(r'^coursemanage/users?$', views.courseman_users, name="courseman_users" ),
    url(r'^coursemanage/users/delete/(?P<id>[\w-]+)/?$', views.courseman_users_delete, name="courseman_users_delete" ),
    url(r'^coursemanage/uploadcsv-post?$', views.courseman_users_uploadcsv_post, name="courseman_users_uploadcsv_post" ),
    url(r'^coursemanage/users-post?$', views.courseman_users_post, name="courseman_users_post" ),
    
    # contact form
    url(r'^contact/?$', views.contact, name="contact"),
]
