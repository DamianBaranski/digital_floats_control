SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $SCRIPT_DIR
DATE=`date +"%y%m%d%H%M%S"`
HASH=`git rev-parse --short HEAD`
TAG="1.0_"
VERSION="$TAG$DATE$HASH"
cd $SCRIPT_DIR
echo $VERSION
echo "#define VERSION \"$VERSION\"" > application/app/version.h
echo "#define VERSION \"$VERSION\"" > bootloader/app/version.h
echo "$VERSION" > version.txt

