#!/bin/bash
#
# Usage: ./macdeploy.sh <full-path-to-macdeployqt>
#
# macdeployqt is usually located in QTDIR/bin/macdeployqt

if [ -z "$1" ]; then
 echo "Required parameter missing for full path to macdeployqt"
 exit 1
fi

MACDEPLOYQT=$1
QTDIR="`dirname $MACDEPLOYQT`/.."
LIBRARY_NAME="libKClientPrivate.2.dylib"
PLUGINS="KClient.app/Contents/Resources/plugins"
QTPLUGINS="KClient.app/Contents/PlugIns"

# cd to directory with bundle
test -d bin || cd ..
cd bin

# copy libKClientPrivate into bundle
cp $LIBRARY_NAME KClient.app/Contents/MacOS/

# copy all KClient plugins into bundle
test -d $PLUGINS || mkdir $PLUGINS
cp plugins/*.dylib $PLUGINS/

# fix libKClient
install_name_tool -change $LIBRARY_NAME @executable_path/$LIBRARY_NAME KClient.app/Contents/MacOS/KClient

# fix plugins
for plugin in $PLUGINS/*.dylib
do
 install_name_tool -change $LIBRARY_NAME @executable_path/$LIBRARY_NAME $plugin
done

# copy known, missing, Qt native library plugins into bundle
#
# See:
#  *  http://code.qt.io/cgit/qt/qttools.git/tree/src/macdeployqt/shared/shared.cpp#n1044
#
mkdir -p $QTPLUGINS

FILE="$QTDIR/plugins/iconengines/libqsvgicon.dylib"
if [ -f "$FILE" ]; then
 cp $FILE $QTPLUGINS/
else
 echo "$FILE: No such file"
 exit 1
fi

# run macdeployqt
$MACDEPLOYQT KClient.app -qmldir=$PWD/../src/lib/data/data

# create final dmg image
cd ../mac
./create_dmg.sh
