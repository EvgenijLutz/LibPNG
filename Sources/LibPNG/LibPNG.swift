// The Swift Programming Language
// https://docs.swift.org/swift-book

import png
import LibPNGC


public func openGlamorousPNG(path: String) -> GlamorousPNG? {
    path.withCString { pathCString in
        openGlamorousPNG(pathCString)
    }
}
