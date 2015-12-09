#!/bin/sh
#
# Wrapper script for aopt from VirtualPlayer (instantreality.org) to allow
# the version for Ubuntu 14.04 LTS to run on other Linux systems.
#

export AOPT_ROOT=/srv/mcweb/McWeb/aopt-sandbox
export LD_LIBRARY_PATH=$AOPT_ROOT/opt/instantReality/lib:$AOPT_ROOT/lib:$AOPT_ROOT/lib/x86_64-linux-gnu/:$AOPT_ROOT/usr/lib:$AOPT_ROOT/usr/lib/x86_64-linux-gnu/
export PATH=$AOPT_ROOT/opt/instantReality/bin:$PATH

aopt $*
