#!/bin/sh
# InstantReality post install script
# IGD/2007

if [ -x `which xdg-open` ] ; then

SHARE_DIR=$1
ICONS_DIR=$SHARE_DIR/icons

#-----------------------------------------------
# set the local xdg-utils path
export PATH=$PATH:$SHARE_DIR/xdg-utils/scripts

#-----------------------------------------------
# uninstall the mime types

# uninstall the application icons

# uninstall the mime $ICONS_DIR

# install application menu entry
xdg-desktop-menu uninstall $SHARE_DIR/instantReality-Player.desktop
xdg-desktop-menu uninstall $SHARE_DIR/instantReality-Cluster.desktop
xdg-desktop-menu uninstall $SHARE_DIR/instantReality-IO.desktop

# install desktop icon
xdg-desktop-icon uninstall $SHARE_DIR/instantReality-Player.desktop
xdg-desktop-icon uninstall $SHARE_DIR/instantReality-Cluster.desktop
xdg-desktop-icon uninstall $SHARE_DIR/instantReality-IO.desktop

fi