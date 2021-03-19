```shell
podman build -t mcweb-py2 .
podman run -p 8008:80 -ti mcweb-py2

# Inside the container
sudo -Hu www-data bash django-setup-test.sh
service uwsgi_mcweb start
service nginx start
```

Go to http://localhost:8008/ and log in as djangotest with the password
you set in the django setup stage.
