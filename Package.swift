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
        .custom("Android", versionString: "5.0")
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
        {
#if os(macOS) || os(iOS) || os(tvOS) || os(watchOS) || os(visionOS)
            .binaryTarget(
                name: "png",
                path: "Binaries/png.xcframework"
            )
#else
            .binaryTarget(
                name: "png",
                path: "Binaries/png.artifactbundle"
            )
#endif
        }(),
        .target(
            name: "LibPNGC",
            dependencies: [
                .target(name: "png")
            ],
            cxxSettings: [
                .enableWarning("all")
            ],
            linkerSettings: [
                // Links libz.tbd that comes with all Apple and Android systems
                .linkedLibrary("z"),
                // Links libbz2.tbd that comes with all Apple systems, but not Android :(
                .linkedLibrary("bz2", .when(platforms: [.macOS, .iOS, .tvOS, .watchOS, .visionOS]))
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
    // The libpng library was compiled using c17, so set it also here
    cLanguageStandard: .c17,
    // Also use c++20, we don't live in the stone age, but still not ready to accept c++23
    cxxLanguageStandard: .cxx20
)
