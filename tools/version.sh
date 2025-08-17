SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $SCRIPT_DIR
DATE=`date +"%y%m%d%H%M%S"`
HASH=`git rev-parse --short HEAD`
GIT_COMMIT=`git rev-parse HEAD`
TAG="1.0_"
APP_VERSION="$TAG$HASH"
HARDWARE_VERSION="6CHv1.0"
cd $SCRIPT_DIR

echo "APP_VERSION: $APP_VERSION"
echo "HARDWARE_VERSION: $HARDWARE_VERSION"
echo "GIT_COMMIT: $GIT_COMMIT"

echo "#define APP_VERSION \"$APP_VERSION\"" > ../application/app/version.h
echo "#define HARDWARE_VERSION \"$HARDWARE_VERSION\"" >> ../application/app/version.h
echo "#define GIT_COMMIT \"$GIT_COMMIT\"" >> ../application/app/version.h

echo "#define APP_VERSION \"$APP_VERSION\"" > ../bootloader/app/version.h
echo "#define HARDWARE_VERSION \"$HARDWARE_VERSION\"" >> ../bootloader/app/version.h
echo "#define GIT_COMMIT \"$GIT_COMMIT\"" >> ../bootloader/app/version.h
echo "$APP_VERSION" > ../version.txt



