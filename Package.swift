// swift-tools-version: 6.1
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "LibPNGFramework",
    platforms: [
        .macOS(.v10_13),
        .iOS(.v12),
        .tvOS(.v12),
        .watchOS(.v8),
        .visionOS(.v1)
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
        .binaryTarget(
            name: "LibPNG",
            path: "LibPNG.xcframework"
        ),
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
