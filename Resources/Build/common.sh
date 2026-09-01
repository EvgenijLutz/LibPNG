# Common helpers


# Define some global variables

# Path to Xcode's "Developer" folder
ft_developer="/Applications/Xcode.app/Contents/Developer"

# Path to Xcode's "Platforms" folder
platforms_path='/Applications/Xcode.app/Contents/Developer/Platforms'

# Your signing identity to sign the xcframework. Execute "security find-identity -v -p codesigning" and select one from the list
xcode_signing_identity=070BA25D98F2A17A61E3E27E31BE64C06F901016

# Android NDK path
ndk_path="/Users/evgenij/Library/Android/sdk/ndk/29.0.14206865"


echo "hello handsome"


# Console output formatting
# https://stackoverflow.com/a/2924755
bold=$(tput bold)
normal=$(tput sgr0)


# Checks if the path exists
assert_path() {
  local path=$1
  if [ ! -d "$path" ]; then
    echo "$path does not exist. Check if the path correct and try again."
    exit 1
  fi
}


# Checks if an error happened recently and terminates if it's true
exit_if_error() {
  local result=$?
  if [ $result -ne 0 ] ; then
     echo "Received an exit code $result, aborting"
     exit 1
  fi
}


# Sets the "framework_target" variable - etermine framework target folder name based on
# platform ("MacOSX", "iPhoneOS", "iPhoneSimulator" and so on) and architecture ("x86_64" or "arm64")
set_framework_target_var() {
  local platform_name=$1
  local arch=$2

  if [[ "$platform_name" == "MacOSX" ]]; then
    framework_target="macos-arm64_x86_64"
  elif [[ "$platform_name" == "iPhoneOS" ]]; then
    framework_target="ios-$arch"
  elif [[ "$platform_name" == "iPhoneSimulator" ]]; then
    framework_target="ios-arm64_x86_64-simulator"
  elif [[ "$platform_name" == "AppleTVOS" ]]; then
    framework_target="tvos-$arch"
  elif [[ "$platform_name" == "AppleTVSimulator" ]]; then
    framework_target="tvos-arm64_x86_64-simulator"
  elif [[ "$platform_name" == "WatchOS" ]]; then
    framework_target="watchos-$arch"
  elif [[ "$platform_name" == "WatchSimulator" ]]; then
    framework_target="watchos-arm64_x86_64-simulator"
  elif [[ "$platform_name" == "XROS" ]]; then
    framework_target="xros-$arch"
  elif [[ "$platform_name" == "XRSimulator" ]]; then
    framework_target="xros-arm64_x86_64-simulator"
  else
    echo "Unknown platform $platform_name"
    exit 1
  fi
}

# Sets the "target_system" variable based on
# platform ("MacOSX", "iPhoneSimulator", "Android" and so on)
set_target_system_var() {
  local platform_name=$1
  
  # Apple
  if [[ "$platform_name" == "MacOSX" ]] || \
    [[ "$platform_name" == "iPhoneOS" ]] || [[ "$platform_name" == "iPhoneSimulator" ]] || \
    [[ "$platform_name" == "AppleTVOS" ]] || [[ "$platform_name" == "AppleTVSimulator" ]] || \
    [[ "$platform_name" == "WatchOS" ]] || [[ "$platform_name" == "WatchSimulator" ]] || \
    [[ "$platform_name" == "XROS" ]] || [[ "$platform_name" == "XRSimulator" ]]; then
    target_system="Apple"
  # Android
  elif [[ "$platform_name" == "Android" ]]; then
    target_system="Android"
  fi
}


# Sets the "host" variable based on
# platform ("MacOSX", "iPhoneOS", "iPhoneSimulator" and so on) and architecture ("x86_64" or "arm64")
set_host_var() {
  local platform_name=$1
  local arch=$2
  
  # Apple
  if [[ "$platform_name" == "MacOSX" ]] || \
    [[ "$platform_name" == "iPhoneOS" ]] || [[ "$platform_name" == "iPhoneSimulator" ]] || \
    [[ "$platform_name" == "AppleTVOS" ]] || [[ "$platform_name" == "AppleTVSimulator" ]] || \
    [[ "$platform_name" == "WatchOS" ]] || [[ "$platform_name" == "WatchSimulator" ]] || \
    [[ "$platform_name" == "XROS" ]] || [[ "$platform_name" == "XRSimulator" ]]; then
    if   [[ "$arch" == "arm64" ]];  then host="arm-apple-darwin"
    elif [[ "$arch" == "x86_64" ]]; then host="x86_64-apple-darwin"
    fi
  # Android
  elif [[ "$platform_name" == "Android" ]]; then
    if   [[ "$arch" == "aarch64" ]];  then host="aarch64-linux-android"
    elif [[ "$arch" == "arm" ]];      then host="arm-linux-androideabi"
    elif [[ "$arch" == "i686" ]];     then host="i686-linux-android"
    elif [[ "$arch" == "riscv64" ]];  then host="riscv64-linux-android"
    elif [[ "$arch" == "x86_64" ]];   then host="x86_64-linux-android"
    fi
  fi
}

