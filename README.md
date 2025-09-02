# LibPNG
Precompiled [libpng](https://github.com/pnggroup/libpng) library for all Apple platforms and architectures, including simulators.

## Installing LibPNG

Add the following dependency to your Package.swift:

```Swift
.package(url: "https://github.com/EvgenijLutz/LibPNG.git", from: "1.6.50-alpha3")
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

## Next steps

Support binaries for other platforms beyond Apple.

# Build
Built with `libpng 1.6.50` and `Xcode 16.4`.

Download libpng and copy the `build-apple-make.sh` and `module.modulemap` files from the `LibPNG/Resources/Build` into the `libpng` directory:
```bash
cp Path/To/LibPNG/Resources/Build/build-apple-make.sh libpng/build-apple-make.sh
cp Path/To/LibPNG/Resources/Build/module.modulemap libpng/module.modulemap
```

Move the the libpng directory:
```bash
cd libpng
```

And execute the script:
```bash
bash build-apple-make.sh
```

When everything succeeds, you get `png.xcframework` in the `build-apple` directory. Voilà!
