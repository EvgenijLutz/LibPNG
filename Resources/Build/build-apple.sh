# bash

# LibPNG

source common.sh

# Get some help
#./configure -help >> configure-help.txt

# Output library name. Determined by the build system. Try to change the name if possible in the future
libname=libpng16
source_name=libpng-1.6.58


# Remove logs if exist
# rm -f "build/log.txt"


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

  # Determine host (arm-apple-darwin, aarch64-linux-android and other) and target system (Apple or Android)
  set_target_system_var $platform
  set_host_var $platform $arch

  # Determine some compiler flags
  # Apple
  if [[ "$target_system" == "Apple" ]]; then
    local sysroot="$ft_developer/Platforms/$platform.platform/Developer/SDKs/$platform.sdk"
    local arch_flags="-arch $arch"
    local target_os_flags="-mtargetos=$min_os"
    export CC="$ft_developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
    export LT_SYS_LIBRARY_PATH="-isysroot $sysroot/usr/include"
    export CPPFLAGS="-I$sysroot/usr/include"
    export CFLAGS="-isysroot $sysroot $arch_flags -std=c17 $target_os_flags -O2"

  # Android
  elif [[ "$target_system" == "Android" ]]; then
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

  dir=$(pwd)

  # Remove previously build foler for the specified platform and architecture if exists
  rm -rf "build/$platform/$arch"

  # Temporary directory
  mkdir -p "build/$platform/$arch/tmp"
  cd "build/$platform/$arch/tmp"
  
  # Configure for the specified platform and architecture
  sh ./../../../../$source_name/configure \
    --prefix $dir/build/$platform/$arch/install \
    --host=$host \
    --enable-static=yes \
    --enable-shared=no \
    --disable-tests \
    --disable-tools
  exit_if_error

  # Build and install
  make install -j$(sysctl -n hw.ncpu)
  exit_if_error

  # Go back
  cd ../../../..

  # Remove temporary data
  rm -rf "build/$platform/$arch/tmp"
  exit_if_error

  # About modules
  # https://clang.llvm.org/docs/Modules.html
  # Without module.modulemap png is not exposed to Swift
  # Copy the module map into the directory with installed header files
  # mkdir -p build/$platform/$arch/install/include/$libname/png-Module
  # cp Contents/module.modulemap build/$platform/$arch/install/include/$libname/png-Module/module.modulemap
  # exit_if_error
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
  rm -rf build/png.xcframework
  exit_if_error

  # Merge macOS arm and x86 binaries
  mkdir -p build/MacOSX
  exit_if_error
  lipo -create -output build/MacOSX/$libname.a \
    build/MacOSX/arm64/install/lib/$libname.a \
    build/MacOSX/x86_64/install/lib/$libname.a
  exit_if_error

  # Merge iOS simulator arm and x86 binaries
  mkdir -p build/iPhoneSimulator
  exit_if_error
  lipo -create -output build/iPhoneSimulator/$libname.a \
    build/iPhoneSimulator/arm64/install/lib/$libname.a \
    build/iPhoneSimulator/x86_64/install/lib/$libname.a
  exit_if_error

  # Merge tvOS simulator arm and x86 binaries
  mkdir -p build/AppleTVSimulator
  exit_if_error
  lipo -create -output build/AppleTVSimulator/$libname.a \
    build/AppleTVSimulator/arm64/install/lib/$libname.a \
    build/AppleTVSimulator/x86_64/install/lib/$libname.a
  exit_if_error

  # Merge watchOS simulator arm and x86 binaries
  mkdir -p build/WatchSimulator
  exit_if_error
  lipo -create -output build/WatchSimulator/$libname.a \
    build/WatchSimulator/arm64/install/lib/$libname.a \
    build/WatchSimulator/x86_64/install/lib/$libname.a
  exit_if_error

  # Merge visionOS simulator arm and x86 binaries
  mkdir -p build/XRSimulator
  exit_if_error
  lipo -create -output build/XRSimulator/$libname.a \
    build/XRSimulator/arm64/install/lib/$libname.a \
    build/XRSimulator/x86_64/install/lib/$libname.a
  exit_if_error

  # Create the framework with multiple platforms
  xcodebuild -create-xcframework \
    -library build/MacOSX/$libname.a              -headers build/MacOSX/arm64/install/include/$libname \
    -library build/iPhoneOS/arm64/install/lib/$libname.a  -headers build/iPhoneOS/arm64/install/include/$libname \
    -library build/iPhoneSimulator/$libname.a     -headers build/iPhoneSimulator/arm64/install/include/$libname \
    -library build/AppleTVOS/arm64/install/lib/$libname.a -headers build/AppleTVOS/arm64/install/include/$libname \
    -library build/AppleTVSimulator/$libname.a    -headers build/AppleTVSimulator/arm64/install/include/$libname \
    -library build/WatchOS/arm64/install/lib/$libname.a   -headers build/WatchOS/arm64/install/include/$libname \
    -library build/WatchSimulator/$libname.a      -headers build/WatchSimulator/arm64/install/include/$libname \
    -library build/XROS/arm64/install/lib/$libname.a      -headers build/XROS/arm64/install/include/$libname \
    -library build/XRSimulator/$libname.a         -headers build/XRSimulator/arm64/install/include/$libname \
    -output build/png.xcframework
  exit_if_error

  # And sign the framework
  codesign --timestamp -s $xcode_signing_identity build/png.xcframework
  exit_if_error
}
create_framework


# Artifact bundle for Android
create_artifactbundle() {
  # Remove previously created artifact if exists
  rm -rf build/png.artifactbundle
  exit_if_error

  # Create the artifact bundle folder
  mkdir -p build/png.artifactbundle
  exit_if_error

  # info.json
  cp Contents/info.json build/png.artifactbundle/info.json
  exit_if_error

  # Headers
  cp -r build/Android/aarch64/install/include build/png.artifactbundle/include
  exit_if_error

  # aarch64-linux-android
  mkdir -p build/png.artifactbundle/aarch64-linux-android
  exit_if_error
  cp build/Android/aarch64/install/lib/$libname.a build/png.artifactbundle/aarch64-linux-android/png.a
  exit_if_error

  # arm-linux-androideabi
  mkdir -p build/png.artifactbundle/arm-linux-androideabi
  exit_if_error
  cp build/Android/arm/install/lib/$libname.a build/png.artifactbundle/arm-linux-androideabi/png.a
  exit_if_error

  # i686-linux-android
  mkdir -p build/png.artifactbundle/i686-linux-android
  exit_if_error
  cp build/Android/i686/install/lib/$libname.a build/png.artifactbundle/i686-linux-android/png.a
  exit_if_error

  # riscv64-linux-android
  mkdir -p build/png.artifactbundle/riscv64-linux-android
  exit_if_error
  cp build/Android/riscv64/install/lib/$libname.a build/png.artifactbundle/riscv64-linux-android/png.a
  exit_if_error

  # x86_64-linux-android
  mkdir -p build/png.artifactbundle/x86_64-linux-android
  exit_if_error
  cp build/Android/x86_64/install/lib/$libname.a build/png.artifactbundle/x86_64-linux-android/png.a
  exit_if_error
}
create_artifactbundle






# Done!
