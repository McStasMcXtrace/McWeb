Setup for testing the web simulator in a container.

These steps build a container image with the code from this repository and all
its dependencies, then run a container with a host port like 8008 mapped to port
80 inside the container. The web app is launched listening on port 80 inside
the container.

I am using [podman](https://podman.io/), which is designed to be a drop-in
replacement for the docker command. If you don't have podman, replace `podman`
with `docker` in all the commands below, and they should still work.

```shell
# Build image & launch container:
podman build -t mcweb-py3 .
podman run -p 8008:80 -ti mcweb-py3

# Inside the container:
sudo -Hu www-data bash django-setup-test.sh
service uwsgi_mcweb start
service nginx start
```

Go to http://localhost:8008/ and log in as djangoadmin with the password
you set in the django setup stage.

There is plenty of room to improve on this, e.g. doing more of the setup
while creating the image, so it's quicker to start containers, and refining
what dependencies are really needed to make a smaller image. It was written as
if it would only be used for a few days.
