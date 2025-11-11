//
//  PNGImage.hpp
//  LibPNG
//
//  Created by Evgenij Lutz on 31.10.25.
//

#pragma once

#include <LibPNGC/Common.hpp>


/// Glamorously decompressed PNG image.
///
/// "It takes a lot of money to look this cheap, darling."
class PNGImage final {
private:
    std::atomic<size_t> _referenceCounter;
    
    /// Pixel buffer.
    const char* fn_nonnull _contents;
    const long _width;
    const long _height;
    const long _numComponents;
    const long _bitsPerComponent;
    
    const bool _sRGB;
    const float _gamma;
    
    const char* fn_nullable _iccData;
    const long _iccDataLength;
    
    FN_FRIEND_SWIFT_INTERFACE(PNGImage)
    
    PNGImage(const char* fn_nonnull contents,
             long width, long height,
             long numComponents, long bitsPerComponent,
             bool sRGB, float gamma,
             char* fn_nullable iccData, long iccDataLength);
    ~PNGImage();
    
    
public:
    /// Checks if a file at the specified `path` is a `png` image.
    static bool checkIfPNG(const char* fn_nonnull path fn_noescape);
    
    /// Loads a png file at the specified `path`.
    ///
    /// - Returns: a valid ``PNGImage`` if the image was successfully loaded, otherwise false.
    [[nodiscard("Don't forget to release the object using PNGImageRelease")]]
    static PNGImage* fn_nullable open(const char* fn_nonnull path fn_noescape) SWIFT_RETURNS_RETAINED SWIFT_NAME(__openUnsafe(path:));
    
    //std::span<char> getData() lifetimebound SWIFT_COMPUTED_PROPERTY;
    const char* fn_nonnull getContents() fn_lifetimebound SWIFT_COMPUTED_PROPERTY { return _contents; }
    long getDataSize() const SWIFT_COMPUTED_PROPERTY { return (_bitsPerComponent / 8) * _numComponents * _width * _height; }
    long getWidth() const SWIFT_COMPUTED_PROPERTY { return _width; }
    long getHeight() const SWIFT_COMPUTED_PROPERTY { return _height; }
    long getNumComponents() const SWIFT_COMPUTED_PROPERTY { return _numComponents; }
    long getBitsPerComponent() const SWIFT_COMPUTED_PROPERTY { return _bitsPerComponent; }
    long getBitsPerPixel() const SWIFT_COMPUTED_PROPERTY { return _bitsPerComponent * _numComponents; }
    long getBitsPerRow() const SWIFT_COMPUTED_PROPERTY { return _bitsPerComponent * _numComponents * _width / 8; }
    
    bool getIsSRGB() const SWIFT_COMPUTED_PROPERTY { return _sRGB; }
    float getGamma() const SWIFT_COMPUTED_PROPERTY { return _gamma; }
    
    const char* fn_nullable getICCPData() fn_lifetimebound SWIFT_NAME(__getICCPDataUnsafe()) { return _iccData; };
    long getICCPDataLength() SWIFT_NAME(_getICCPDataLengthUnsafe()) { return _iccDataLength; };
}
SWIFT_PRIVATE_FILEID("LibPNG/LibPNG.swift")
FN_SWIFT_INTERFACE(PNGImage)
SWIFT_UNCHECKED_SENDABLE;


FN_DEFINE_SWIFT_INTERFACE(PNGImage)
