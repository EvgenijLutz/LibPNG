# bash

# LibPNG


# LLVM cross-compile
# https://llvm.org/docs/HowToCrossCompileLLVM.html


# Target triplets and cross-compilation using Clang
# https://clang.llvm.org/docs/CrossCompilation.html#target-triple


# Build for Android using other build systems
# https://developer.android.com/ndk/guides/other_build_systems


# Binary Static Library Dependencies
# https://github.com/swiftlang/swift-evolution/blob/main/proposals/0482-swiftpm-static-library-binary-target-non-apple-platforms.md


# Get some help
#./configure -help >> configure-help.txt

# Define some global variables
ft_developer="/Applications/Xcode.app/Contents/Developer"
# Output library name. Determined by the build system. Try to change the name if possible in the future
libname=libpng16
# Your signing identity to sign the xcframework. Execute "security find-identity -v -p codesigning" and select one from the list
identity=B42A10624E8E06BC95CD03069100C6E67121D61B

# Android NDK path
ndk_path="/Users/evgenij/Library/Android/sdk/ndk/29.0.13846066"


# Console output formatting
# https://stackoverflow.com/a/2924755
bold=$(tput bold)
normal=$(tput sgr0)


# Remove logs if exist
# rm -f "build-apple/log.txt"


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
  local min_os=$3

  # Reset variables
  export LT_SYS_LIBRARY_PATH=""
  export AR=""
  export CC=""
  export AS=""
  export CXX=""
  export LD=""
  export RANLIB=""
  export STRIP=""
  export CPPFLAGS=""
  export CFLAGS=""

  # Determine host based on platform and architecture
  # Apple
  if [[ "$platform" == "MacOSX" ]] || \
    [[ "$platform" == "iPhoneOS" ]] || [[ "$platform" == "iPhoneSimulator" ]] || \
    [[ "$platform" == "AppleTVOS" ]] || [[ "$platform" == "AppleTVSimulator" ]] || \
    [[ "$platform" == "WatchOS" ]] || [[ "$platform" == "WatchSimulator" ]] || \
    [[ "$platform" == "XROS" ]] || [[ "$platform" == "XRSimulator" ]]; then
    if   [[ "$arch" == "arm64" ]];  then local host="arm-apple-darwin"
    elif [[ "$arch" == "x86_64" ]]; then local host="x86_64-apple-darwin"
    fi

    local sysroot="$ft_developer/Platforms/$platform.platform/Developer/SDKs/$platform.sdk"
    local arch_flags="-arch $arch"
    local target_os_flags="-mtargetos=$min_os"
    export CC="$ft_developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
    export LT_SYS_LIBRARY_PATH="-isysroot $sysroot/usr/include"
    export CPPFLAGS="-I$sysroot/usr/include"
    export CFLAGS="-isysroot $sysroot $arch_flags -std=c17 $target_os_flags -O2"

  # Android
  elif [[ "$platform" == "Android" ]]; then
    if   [[ "$arch" == "aarch64" ]];  then local host="aarch64-linux-android"
    elif [[ "$arch" == "arm" ]];      then local host="arm-linux-androideabi"
    elif [[ "$arch" == "i686" ]];     then local host="i686-linux-android"
    elif [[ "$arch" == "riscv64" ]];  then local host="riscv64-linux-android"
    elif [[ "$arch" == "x86_64" ]];   then local host="x86_64-linux-android"
    fi

    local sysroot="$ndk_path/toolchains/llvm/prebuilt/darwin-x86_64/sysroot"
    local arch_flags=""
    local target_os_flags="--target=$host$min_os"
    export LT_SYS_LIBRARY_PATH=""

    local toolchain="$ndk_path/toolchains/llvm/prebuilt/darwin-x86_64"
    export AR=$toolchain/bin/llvm-ar
    export CC="$toolchain/bin/clang $target_os_flags"
    export AS=$CC
    export CXX="$toolchain/bin/clang++ $target_os_flags"
    export LD=$toolchain/bin/ld
    export RANLIB=$toolchain/bin/llvm-ranlib
    export STRIP=$toolchain/bin/llvm-strip

    export CPPFLAGS=""
    export CFLAGS="-std=c17 -O2"

  else
    echo "Unknown platform $platform"
    exit 1
  fi

  # Welcome message
  echo "Build for ${bold}$platform $host${normal}"

  # Are we sure that we need to specify the architecture for the linker?
  export LDFLAGS="$arch_flags"

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

  # About modules
  # https://clang.llvm.org/docs/Modules.html
  # Without module.modulemap png is not exposed to Swift
  # Copy the module map into the directory with installed header files
  mkdir -p build-apple/$platform/$arch/include/$libname/png-Module
  cp module.modulemap build-apple/$platform/$arch/include/$libname/png-Module/module.modulemap
  exit_if_error
}

# Build for Apple systems
build_library MacOSX           arm64  macos11
build_library MacOSX           x86_64 macos10.13
build_library iPhoneOS         arm64  ios12
build_library iPhoneSimulator  arm64  ios14-simulator
build_library iPhoneSimulator  x86_64 ios12-simulator
build_library AppleTVOS        arm64  tvos12
build_library AppleTVSimulator arm64  tvos12-simulator
build_library AppleTVSimulator x86_64 tvos12-simulator
build_library WatchOS          arm64  watchos8
build_library WatchSimulator   arm64  watchos8-simulator
build_library WatchSimulator   x86_64 watchos8-simulator
build_library XROS             arm64  xros1
build_library XRSimulator      arm64  xros1-simulator
build_library XRSimulator      x86_64 xros1-simulator

# Build for Android
build_library Android aarch64 21
build_library Android arm     21
build_library Android i686    21
build_library Android riscv64 35
build_library Android x86_64  21


create_framework() {
  # Remove previously created framework if exists
  rm -rf build-apple/png.xcframework
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
    -output build-apple/png.xcframework
  exit_if_error

  # And sign the framework
  codesign --timestamp -s $identity build-apple/png.xcframework
  exit_if_error
}
create_framework






# Done!