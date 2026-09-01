# LibPNG

[![](https://img.shields.io/endpoint?url=https%3A%2F%2Fswiftpackageindex.com%2Fapi%2Fpackages%2FEvgenijLutz%2FLibPNG%2Fbadge%3Ftype%3Dswift-versions)](https://swiftpackageindex.com/EvgenijLutz/LibPNG) [![](https://img.shields.io/endpoint?url=https%3A%2F%2Fswiftpackageindex.com%2Fapi%2Fpackages%2FEvgenijLutz%2FLibPNG%2Fbadge%3Ftype%3Dplatforms)](https://swiftpackageindex.com/EvgenijLutz/LibPNG)

Precompiled [libpng](https://github.com/pnggroup/libpng) library for all Apple platforms and architectures, including simulators. The package also contains helper interfaces in C++ with Swift interoperability.

## Installing LibPNG

Add the following dependency to your Package.swift:

```Swift
.package(url: "https://github.com/EvgenijLutz/LibPNG.git", from: "1.6.58")
```

And you're good to go!

## Dependencies

`libpng` relies on zlib (general purpose lossless data compression) and bz2 (compression method that uses the Burrows–Wheeler algorithm) libraries, that are already shipped with all Apple operating systems. Client applications need to link these two libraries in order to compile successfully, which is already taken care by this package:

```Swift
linkerSettings: [
    // Links libz.tbd that comes with all Apple systems
    .linkedLibrary("z"),
    // Links libbz2.tbd that comes with all Apple systems
    .linkedLibrary("bz2")
]
```

## Package contents

Besides the binary and original API, the package contains additional helper interfaces in C++ and Swift.

```C++
#include <LibPNGC/LibPNGC.hpp>

void readSomeInfos(const char* path) {
    // Open an image
    auto png = PNGImage::open(path);
    
    // Check if it's an sRGB image
    auto isSRGB = png->getIsSRGB();
    // do something...
    
    // Get ICC profile data
    auto icc = png->getICCPData();
    // do something...
    
    // Release image
    PNGImageRelease(png);
}

int saveRGBA8Image(const char* contents, long width, long height, const char* outPath) {
    // Create an image
    auto png = PNGImage::create(contents, width, height, 4, 8);  // RGBA8 pixel format
    
    // Save image with the middle compression level
    auto result = png->write(outPath, {
        .compressionLevel = 0.5f
    });
    
    // Release image
    PNGImageRelease(png);
    
    // 0 if succeeded
    return result;
}
```

```Swift
import LibPNG

func readSomeInfos(path: String) {
    // Open an image
    let png = PNGImage.open(path: path)!
    
    // Check if it's an sRGB image
    let isSRGB = png.issrgb
    // do something...
    
    // Get ICC profile data
    let icc = png.iccData
    // do something...
}

func saveRGBA8Image(contents: Data, width: Int, height: Int, outPath: String) -> Int {
    // Create an image
    let png = PNGImage.from(contents, width: width, height: height, numComponents: 4, bitsPerComponent: 8)!
    
    // Save image with the middle compression level
    return png.write(to: outPath, compressionLevel: 0.5)
}
```

## Next steps

Support binaries for other platforms beyond Apple.

# Build
Built with `libpng 1.6.58` and `Xcode 26.6`.

Download [libpng source code archive](https://github.com/pnggroup/libpng/releases/tag/v1.6.58), extract it and copy the `build-apple.sh` and `common.sh` files from the `LibPNG/Resources/Build` into the `libpng` directory:
```bash
cp Path/To/LibPNG/Resources/Build/build-apple.sh libpng/build-apple.sh
cp Path/To/LibPNG/Resources/Build/common.sh libpng/common.sh
```

In the `build-apple.sh` script, change the `source_name` variable to the name of the folder with extracted source code (for instance, `libpng-1.6.58` in our case). In the `common.sh` script, set the `xcode_signing_identity` and `ndk_path` variables to your xcode signing identity (instruction how to get it in the comments) and Android NDK path.

And then, execute the script:
```bash
bash build-apple.sh
```

When everything succeeds, you get `png.xcframework` and `png.artifactbundle` in the `build` directory. Voilà!
