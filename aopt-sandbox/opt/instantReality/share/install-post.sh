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
# install the mime types
xdg-mime install $SHARE_DIR/instantReality-mimeInfo.xml

# install the application icons
xdg-icon-resource install --size 128 $ICONS_DIR/instantReality-Player.png
xdg-icon-resource install --size 128 $ICONS_DIR/instantReality-Cluster.png
xdg-icon-resource install --size 128 $ICONS_DIR/instantReality-IO.png

# install the mime $ICONS_DIR
xdg-icon-resource install --novendor --context mimetypes --size 128 $ICONS_DIR/x3d-icon.png model-x3d+xml
xdg-icon-resource install --novendor --context mimetypes --size 128 $ICONS_DIR/x3d-icon.png model-x3d+vrml
xdg-icon-resource install --novendor --context mimetypes --size 128 $ICONS_DIR/x3d-icon.png model-x3d+binary
xdg-icon-resource install --novendor --context mimetypes --size 128 $ICONS_DIR/vrml-icon.png model-vrml
xdg-icon-resource install --novendor --context mimetypes --size 128 $ICONS_DIR/avalon-icon.png application-x-avalon-engine+vrml
xdg-icon-resource install --novendor --context mimetypes --size 128 $ICONS_DIR/avalon-icon.png application-x-avalon-engine+xml
xdg-icon-resource install --novendor --context mimetypes --size 128 $ICONS_DIR/avalon-icon.png application-x-avalon-engine+binary
xdg-icon-resource install --novendor --context mimetypes --size 128 $ICONS_DIR/license-icon.png application-x-irlicense
xdg-icon-resource install --novendor --context mimetypes --size 128 $ICONS_DIR/opensg-icon.png application-x-opensg+ascii
xdg-icon-resource install --novendor --context mimetypes --size 128 $ICONS_DIR/opensg-icon.png application-x-opensg+binary

# install default application ?!?
#xdg-mime default $SHARE_DIR/instantReality-Player.desktop model/vrml
#xdg-mime default $SHARE_DIR/instantReality-Player.desktop model/x3d+xml
#xdg-mime default $SHARE_DIR/instantReality-Player.desktop model/x3d+binary
#xdg-mime default $SHARE_DIR/instantReality-Player.desktop model/x3d+vrml

# install application menu entry
xdg-desktop-menu install $SHARE_DIR/instantReality-Player.desktop
xdg-desktop-menu install $SHARE_DIR/instantReality-Cluster.desktop
xdg-desktop-menu install $SHARE_DIR/instantReality-IO.desktop

# install desktop icon
xdg-desktop-icon install $SHARE_DIR/instantReality-Player.desktop
xdg-desktop-icon install $SHARE_DIR/instantReality-Cluster.desktop
xdg-desktop-icon install $SHARE_DIR/instantReality-IO.desktop

fi