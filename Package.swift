// swift-tools-version: 6.1
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "LibPNG",
    // See the "Minimum Deployment Version for Reference Types Imported from C++":
    // https://www.swift.org/documentation/cxx-interop/status/
    platforms: [
        .macOS(.v14),
        .iOS(.v17),
        .tvOS(.v17),
        .watchOS(.v10),
        .visionOS(.v1)
    ],
    products: [
        //.library(
        //    name: "LibPNGExamples",
        //    targets: ["LibPNGExamples"]
        //),
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
        ),
        .target(
            name: "LibPNGC",
            dependencies: [
                .target(name: "png")
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
                .interoperabilityMode(.Cxx)
            ]
        )//,
        //.target(
        //    name: "LibPNGExamples",
        //    dependencies: [
        //        .target(name: "LibPNG")
        //    ],
        //    resources: [
        //        .process("Resources/")
        //    ],
        //    swiftSettings: [
        //        .interoperabilityMode(.Cxx),
        //        //.unsafeFlags(["-parse-as-library"])
        //    ]
        //)
    ],
    // The libpng library was compiled using c17, so set it also here
    cLanguageStandard: .c17,
    // Also use c++20, we don't live in the stone age, but still not ready to accept c++23
    cxxLanguageStandard: .cxx20
)
