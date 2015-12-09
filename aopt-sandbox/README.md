This folder contains InstantPlayer from instantreality.org plus needed libraries to run.

It has been generated via a Ubuntu 14.04 LTS debootstrap chroot.

To use the aopt tool with mcsimrunner_ng, please

1) Add a link to ./aopt.sh in e.g. /usr/local/bin

2) Configure mcweb/settings.py with
USE_AOPT = True
AOPT_CMD = "aopt.sh"

