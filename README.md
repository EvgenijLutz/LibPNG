# LibPNGFramework
Precompiled libpng library for all Apple platforms and architectures, including simulators.

## Installing LibPNGFramework

Add the following dependency to your Package.swift:

```Swift
.package(url: "https://github.com/EvgenijLutz/LibPNGFramework.git", from: "1.6.50-alpha1")
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

Download libpng and copy the `build-apple.sh` file from the `LibPNGFramework/Resources` into the `libpng` directory:
```bash
cp Path/To/LibPNGFramework/Resources/build-apple.sh libpng/build-apple.sh
```

Move the the libpng directory:
```bash
cd libpng
```

And execute the script:
```bash
bash build-apple.sh
```

When everything succeeds, you get `LibPNG.xcframework` in the `build-apple` directory. Enjoy!