# LibPNGFramework
Precompiled [libpng](https://github.com/pnggroup/libpng) library for all Apple platforms and architectures, including simulators.

## Installing LibPNGFramework

Add the following dependency to your Package.swift:

```Swift
.package(url: "https://github.com/EvgenijLutz/LibPNGFramework.git", from: "1.6.50-alpha2")
```

You will likely need to link the following libraries in your Xcode project (already included in Xcode):
```Plain
libz.tbd
libbz2.1.0.tbd
```

And you're good to go!

## Next steps

Support binaries for other platforms beyond Apple.

# Build
Built with `libpng 1.6.50` and `Xcode 16.4`.

Download libpng and copy the `build-apple-make.sh` and `module.modulemap` files from the `LibPNGFramework/Resources/Build` into the `libpng` directory:
```bash
cp Path/To/LibPNGFramework/Resources/Build/build-apple-make.sh libpng/build-apple-make.sh
cp Path/To/LibPNGFramework/Resources/Build/module.modulemap libpng/module.modulemap
```

Move the the libpng directory:
```bash
cd libpng
```

And execute the script:
```bash
bash build-apple-make.sh
```

When everything succeeds, you get `LibPNG.xcframework` in the `build-apple` directory. Voilà!