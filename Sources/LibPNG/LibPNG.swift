//
//  LibPNG.swift
//  LibPNG
//
//  Created by Evgenij Lutz on 06.08.25.
//

import Foundation
@_exported import LibPNGC


@available(macOS 13.3, iOS 16.4, tvOS 16.4, watchOS 9.4, visionOS 1.0, *)
public extension PNGImage {
    static func open(path: String) -> PNGImage? {
        unsafe path.withCString { pathCString in
            unsafe PNGImage.__openUnsafe(path: pathCString)
        }
    }
    
    
    // fucks up the Swift compiler
//    convenience init(contents: UnsafeRawPointer, width: Int, height: Int,
//                     numComponents: Int, bitsPerComponent: Int,
//                     sRGB: Bool = true, gamma: Float = -1,
//                     iccData: UnsafeRawPointer? = nil, iccDataLength: Int = 0) {
//        self.init(contents, width, height,
//                  numComponents, bitsPerComponent,
//                  sRGB, gamma,
//                  iccData, iccDataLength)
//    }
    
    
    static func from(_ contents: Data, width: Int, height: Int,
                     numComponents: Int, bitsPerComponent: Int,
                     sRGB: Bool = true, gamma: Float = -1,
                     iccData: Data? = nil) -> PNGImage? {
        return contents.withUnsafeBytes { (contentsPointer: UnsafeRawBufferPointer) in
            guard let contentsAddress = contentsPointer.baseAddress else {
                return nil
            }
            
            func result(iccData: UnsafeRawPointer? = nil, iccDataLength: Int = 0) -> PNGImage {
                .init(contentsAddress, width, height,
                      numComponents, bitsPerComponent,
                      sRGB, gamma,
                      iccData, iccDataLength)
            }
            
            if let iccData {
                return iccData.withUnsafeBytes { (iccDataPointer: UnsafeRawBufferPointer) in
                    guard let iccDataAddress = iccDataPointer.baseAddress else {
                        return result()
                    }
                    
                    return result(iccData: iccDataAddress, iccDataLength: iccData.count)
                }
            }
            
            return result()
        }
    }
    
    
    func write(to path: String, compressionLevel: Float = 0.5) -> Int {
        let settings = WriteSettings(compressionLevel: compressionLevel)
        
        return path.withCString { cString in
            return __writeUnsafe(cString, settings)
        }
    }
    
    
    //func withICCData(_ body: (_ data: Data?) throws -> ()) rethrows {
    //    guard let _iccData else {
    //        try body(nil)
    //        return
    //    }
    //
    //    Data(
    //    let data = Data(bytesNoCopy: _iccData, count: _iccDataLength, deallocator: .none)
    //    try body(data)
    //}
    
    
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


@available(macOS 13.3, iOS 16.4, tvOS 16.4, watchOS 9.4, visionOS 1.0, *)
func readSomeInfos(path: String) {
    // Open an image
    let png = PNGImage.open(path: path)!
    
    // Check if it's an sRGB image
    let isSRGB = png.issrgb
    // do something...
    
    // Get ICC profile data
    let icc = png.iccData
    // do something...
}

@available(macOS 13.3, iOS 16.4, tvOS 16.4, watchOS 9.4, visionOS 1.0, *)
func saveRGBA8Image(contents: Data, width: Int, height: Int, outPath: String) -> Int {
    // Create an image
    let png = PNGImage.from(contents, width: width, height: height, numComponents: 4, bitsPerComponent: 8)!
    
    // Save image with the middle compression level
    return png.write(to: outPath, compressionLevel: 0.5)
}
