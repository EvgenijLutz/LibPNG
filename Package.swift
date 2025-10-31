// swift-tools-version: 6.2
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "LibPNG",
    // See the "Minimum Deployment Version for Reference Types Imported from C++":
    // https://www.swift.org/documentation/cxx-interop/status/
    platforms: [
        .macOS(.v10_13),
        .iOS(.v12),
        .tvOS(.v12),
        .watchOS(.v8),
        .visionOS(.v1),
        .custom("Android", versionString: "12.0")
    ],
    products: [
        .library(
            name: "LibPNG",
            targets: ["LibPNG"]
        ),
        .library(
            name: "LibPNGC",
            targets: ["LibPNGC"]
        ),
        .library(
            name: "png",
            targets: ["png"]
        ),
    ],
    targets: [
        .binaryTarget(
            name: "png",
            path: "Binaries/png.xcframework"
            //path: "Binaries/png.artifactbundle"
        ),
        .target(
            name: "LibPNGC",
            dependencies: [
                .target(name: "png")
            ],
            cxxSettings: [
                .enableWarning("all")
            ],
            linkerSettings: [
                // Links libz.tbd that comes with all Apple systems
                .linkedLibrary("z"),
                // Links libbz2.tbd that comes with all Apple systems
                .linkedLibrary("bz2")
            ]
        ),
        .target(
            name: "LibPNG",
            dependencies: [
                .target(name: "LibPNGC")
            ],
            swiftSettings: [
                .interoperabilityMode(.Cxx),
                //.strictMemorySafety()
            ]
        )
    ],
//    swiftLanguageVersions: [
//        // C++ interoperability is supported in Swift 5.9 and above
//        .version("5.9")
//    ],
    // The libpng library was compiled using c17, so set it also here
    cLanguageStandard: .c17,
    // Also use c++20, we don't live in the stone age, but still not ready to accept c++23
    cxxLanguageStandard: .cxx20
)
