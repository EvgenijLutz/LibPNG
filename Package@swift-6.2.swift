// swift-tools-version: 6.2
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

func libPNGTarget() -> Target {
#if os(Android)
    .binaryTarget(
        name: "LibPNG",
        path: "Binaries/LibPNG.artifactbundle"
    )
#else
    .binaryTarget(
        name: "LibPNG",
        path: "Binaries/LibPNG.xcframework"
    )
#endif
}

let package = Package(
    name: "LibPNGFramework",
    platforms: [
        .macOS(.v10_13),
        .iOS(.v12),
        .tvOS(.v12),
        .watchOS(.v8),
        .visionOS(.v1),
        .custom("Android", versionString: "21")
    ],
    products: [
        .library(
            name: "LibPNGFramework",
            targets: ["LibPNGFramework"]
        ),
        .library(
            name: "LibPNGC",
            targets: ["LibPNGC"]
        ),
        .library(
            name: "LibPNG",
            targets: ["LibPNG"]
        ),
    ],
    targets: [
        libPNGTarget(),
        .target(
            name: "LibPNGC",
            dependencies: [
                .target(name: "LibPNG")
            ]
        ),
        .target(
            name: "LibPNGFramework",
            dependencies: [
                .target(name: "LibPNGC")
            ],
            swiftSettings: [
                .interoperabilityMode(.Cxx)
            ]
        ),
    ]
)
