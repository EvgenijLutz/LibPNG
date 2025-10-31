//
//  PNGImage.hpp
//  LibPNG
//
//  Created by Evgenij Lutz on 31.10.25.
//

#pragma once

#include <stdio.h>
#include <atomic>
#include <span>
#include <swift/bridging>


#ifndef nullable
#define nullable __nullable
#endif

#ifndef nonnull
#define nonnull __nonnull
#endif

#ifndef noescape
#define noescape _LIBCPP_NOESCAPE
#endif

#ifndef lifetimebound
#define lifetimebound _LIBCPP_LIFETIMEBOUND
#endif


/// Glamorously decompressed PNG image.
///
/// "It takes a lot of money to look this cheap, darling."
class PNGImage final {
private:
    std::atomic<size_t> referenceCounter;
    
    /// Pixel buffer.
    char* nonnull _contents;
    long _width;
    long _height;
    long _numComponents;
    long _bitsPerComponent;
    
    bool _sRGB;
    float _gamma;
    
    /*const*/ char* nullable _iccData;
    long _iccDataLength;
    
    
    friend PNGImage* nonnull PNGImageRetain(PNGImage* nonnull png) SWIFT_RETURNS_RETAINED;
    friend void PNGImageRelease(PNGImage* nonnull png);
    
    PNGImage(char* nonnull contents,
             long width, long height,
             long numComponents, long bitsPerComponent,
             bool sRGB, float gamma,
             char* nullable iccData, long iccDataLength);
    ~PNGImage();
    
    
public:
    [[nodiscard("Don't forget to release the object using PNGImageRelease")]]
    static PNGImage* nullable open(const char* nonnull path noescape) SWIFT_RETURNS_RETAINED SWIFT_NAME(__openUnsafe(path:));
    
    static bool checkIfPNG(const char* nonnull path noescape);
    
    //std::span<char> getData() lifetimebound SWIFT_COMPUTED_PROPERTY;
    char* nonnull getContents() SWIFT_COMPUTED_PROPERTY { return _contents; }
    long getDataSize() const SWIFT_COMPUTED_PROPERTY { return (_bitsPerComponent / 8) * _numComponents * _width * _height; }
    long getWidth() const SWIFT_COMPUTED_PROPERTY { return _width; }
    long getHeight() const SWIFT_COMPUTED_PROPERTY { return _height; }
    long getNumComponents() const SWIFT_COMPUTED_PROPERTY { return _numComponents; }
    long getBitsPerComponent() const SWIFT_COMPUTED_PROPERTY { return _bitsPerComponent; }
    long getBitsPerPixel() const SWIFT_COMPUTED_PROPERTY { return _bitsPerComponent * _numComponents; }
    long getBitsPerRow() const SWIFT_COMPUTED_PROPERTY { return _bitsPerComponent * _numComponents * _width / 8; }
    
    bool getIsSRGB() const SWIFT_COMPUTED_PROPERTY { return _sRGB; }
    float getGamma() const SWIFT_COMPUTED_PROPERTY { return _gamma; }
    
    const char* nullable getICCPData() SWIFT_NAME(_getICCPDataUnsafe()) { return _iccData; };
    long getICCPDataLength() SWIFT_NAME(_getICCPDataLengthUnsafe()) { return _iccDataLength; };
}
SWIFT_PRIVATE_FILEID("LibPNG/LibPNG.swift")
SWIFT_SHARED_REFERENCE(PNGImageRetain, PNGImageRelease)
SWIFT_UNCHECKED_SENDABLE;
