# bash

# Get some help
#./configure -help >> configure-help.txt

# Define some global variables
ft_developer="/Applications/Xcode.app/Contents/Developer"
libname=libpng16
# Your signing identity to sign the xcframework. Execute "security find-identity -v -p codesigning" and select one from the list
identity=YOUR_SIGNING_IDENTITY


# Remove logs if exist
rm -f "build-apple/log.txt"


exit_if_error() {
  local result=$?
  if [ $result -ne 0 ] ; then
     echo "Received an exit code $result, aborting"
     exit 1
  fi
}


build_library() {
  local platform=$1
  local arch=$2
  local host=$3
  local min_os=$4

  local sysroot="$ft_developer/Platforms/$platform.platform/Developer/SDKs/$platform.sdk"

  export CC="$ft_developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
  export LT_SYS_LIBRARY_PATH="-isysroot $sysroot/usr/include"
  export CFLAGS="-isysroot $sysroot -arch $arch -std=c17 -mtargetos=$min_os -Os"
  export CPPFLAGS="-I$sysroot/usr/include"
  # Are we sure that we need to specify the architecture for the linker?
  export LDFLAGS="-arch $arch"

  # Clean previous setup and build if exists
  make clean
  exit_if_error

  # Remove previously build foler for the specified platform and architecture if exists
  rm -f "build-apple/$platform/$arch"
  
  # Configure for the specified platform and architecture
  ./configure \
    --prefix $(pwd)/build-apple/$platform/$arch \
    --host=$host \
    --enable-static=yes \
    --enable-shared=no \
    --disable-tests \
    --disable-tools
  exit_if_error

  # Build
  make
  exit_if_error

  # Install
  make install
  exit_if_error
}

build_library MacOSX arm64     arm-apple-darwin    macos11
build_library MacOSX x86_64    x86_64-apple-darwin macos10.12
build_library iPhoneOS         arm64  arm-apple-darwin    ios12
build_library iPhoneSimulator  arm64  arm-apple-darwin    ios14-simulator
build_library iPhoneSimulator  x86_64 x86_64-apple-darwin ios12-simulator
build_library AppleTVOS        arm64  arm-apple-darwin    tvos12
build_library AppleTVSimulator arm64  arm-apple-darwin    tvos12-simulator
build_library AppleTVSimulator x86_64 x86_64-apple-darwin tvos12-simulator
build_library WatchOS          arm64  arm-apple-darwin    watchos8
build_library WatchSimulator   arm64  arm-apple-darwin    watchos8-simulator
build_library WatchSimulator   x86_64 x86_64-apple-darwin watchos8-simulator
build_library XROS             arm64  arm-apple-darwin    xros1
build_library XRSimulator      arm64  arm-apple-darwin    xros1-simulator
build_library XRSimulator      x86_64 x86_64-apple-darwin xros1-simulator


create_framework() {
  # Remove previously created framework if exists
  rm -rf build-apple/LibPNG.xcframework
  exit_if_error

  # Merge macOS arm and x86 binaries
  mkdir -p build-apple/MacOSX
  exit_if_error
  lipo -create -output build-apple/MacOSX/$libname.a \
    build-apple/MacOSX/arm64/lib/$libname.a \
    build-apple/MacOSX/x86_64/lib/$libname.a
  exit_if_error

  # Merge iOS simulator arm and x86 binaries
  mkdir -p build-apple/iPhoneSimulator
  exit_if_error
  lipo -create -output build-apple/iPhoneSimulator/$libname.a \
    build-apple/iPhoneSimulator/arm64/lib/$libname.a \
    build-apple/iPhoneSimulator/x86_64/lib/$libname.a
  exit_if_error

  # Merge tvOS simulator arm and x86 binaries
  mkdir -p build-apple/AppleTVSimulator
  exit_if_error
  lipo -create -output build-apple/AppleTVSimulator/$libname.a \
    build-apple/AppleTVSimulator/arm64/lib/$libname.a \
    build-apple/AppleTVSimulator/x86_64/lib/$libname.a
  exit_if_error

  # Merge watchOS simulator arm and x86 binaries
  mkdir -p build-apple/WatchSimulator
  exit_if_error
  lipo -create -output build-apple/WatchSimulator/$libname.a \
    build-apple/WatchSimulator/arm64/lib/$libname.a \
    build-apple/WatchSimulator/x86_64/lib/$libname.a
  exit_if_error

  # Merge visionOS simulator arm and x86 binaries
  mkdir -p build-apple/XRSimulator
  exit_if_error
  lipo -create -output build-apple/XRSimulator/$libname.a \
    build-apple/XRSimulator/arm64/lib/$libname.a \
    build-apple/XRSimulator/x86_64/lib/$libname.a
  exit_if_error

  # Create the framework with multiple platforms
  xcodebuild -create-xcframework \
    -library build-apple/MacOSX/$libname.a              -headers build-apple/MacOSX/arm64/include/$libname \
    -library build-apple/iPhoneOS/arm64/lib/$libname.a  -headers build-apple/iPhoneOS/arm64/include/$libname \
    -library build-apple/iPhoneSimulator/$libname.a     -headers build-apple/iPhoneSimulator/arm64/include/$libname \
    -library build-apple/AppleTVOS/arm64/lib/$libname.a -headers build-apple/AppleTVOS/arm64/include/$libname \
    -library build-apple/AppleTVSimulator/$libname.a    -headers build-apple/AppleTVSimulator/arm64/include/$libname \
    -library build-apple/WatchOS/arm64/lib/$libname.a   -headers build-apple/WatchOS/arm64/include/$libname \
    -library build-apple/WatchSimulator/$libname.a      -headers build-apple/WatchSimulator/arm64/include/$libname \
    -library build-apple/XROS/arm64/lib/$libname.a      -headers build-apple/XROS/arm64/include/$libname \
    -library build-apple/XRSimulator/$libname.a         -headers build-apple/XRSimulator/arm64/include/$libname \
    -output build-apple/LibPNG.xcframework
  exit_if_error

  # And sign the framework
  codesign --timestamp -s $identity build-apple/LibPNG.xcframework
  exit_if_error
}
create_framework