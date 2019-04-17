#!/usr/bin/env bash

KCLIENT_URL=$1
RUNTIME_URL="https://github.com/AppImage/AppImageKit/releases/download/continuous/runtime-x86_64"

if [ -z "$KCLIENT_URL" ]; then
    echo "No url specified!"
    exit 1
fi

source /root/env.sh
source /opt/rh/devtoolset-4/enable

cd /root

wget $RUNTIME_URL -O runtime

wget $KCLIENT_URL -O kclient.tar.xz
tar xf kclient.tar.xz
cd kclient-*

/root/build-appimage.sh \
    --sourcedir=`pwd` \
    --outdir=/out \
    --runtime=/root/runtime \
    --qmake=$QTDIR/bin/qmake
