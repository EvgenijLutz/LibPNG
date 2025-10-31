// The Swift Programming Language
// https://docs.swift.org/swift-book

import Foundation
@_exported import LibPNGC
//import png


@available(macOS 13.3, iOS 16.4, tvOS 16.4, watchOS 9.4, visionOS 1.0, *)
public extension PNGImage {
    static func open(path: String) -> PNGImage? {
        unsafe path.withCString { pathCString in
            unsafe PNGImage.__openUnsafe(path: pathCString)
        }
    }
    
    func withICCData(_ body: (_ data: Data?) throws -> ()) rethrows {
        guard let _iccData else {
            try body(nil)
            return
        }
        
        let data = Data(bytesNoCopy: _iccData, count: _iccDataLength, deallocator: .none)
        try body(data)
    }
    
    var iccData: Data? {
        guard let _iccData else {
            return nil
        }
        
        return Data(bytes: _iccData, count: _iccDataLength)
    }
}


#if hasFeature(LifetimeDependency)

@available(macOS 13.3, iOS 16.4, tvOS 16.4, watchOS 9.4, visionOS 1.0, *)
public extension PNGImage {
    @_lifetime(self) func takeSpan() -> Span<UInt8> {
        fatalError("Not implemented")
    }
}

#endif
