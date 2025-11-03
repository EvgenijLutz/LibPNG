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


#ifndef png_nullable
#define png_nullable __nullable
#endif

#ifndef png_nonnull
#define png_nonnull __nonnull
#endif

#ifndef png_noescape
#define png_noescape _LIBCPP_NOESCAPE
#endif

#ifndef png_lifetimebound
#define png_lifetimebound _LIBCPP_LIFETIMEBOUND
#endif


/// Glamorously decompressed PNG image.
///
/// "It takes a lot of money to look this cheap, darling."
class PNGImage final {
private:
    std::atomic<size_t> referenceCounter;
    
    /// Pixel buffer.
    char* png_nonnull _contents;
    long _width;
    long _height;
    long _numComponents;
    long _bitsPerComponent;
    
    bool _sRGB;
    float _gamma;
    
    /*const*/ char* png_nullable _iccData;
    long _iccDataLength;
    
    
    friend PNGImage* png_nonnull PNGImageRetain(PNGImage* png_nonnull png) SWIFT_RETURNS_RETAINED;
    friend void PNGImageRelease(PNGImage* png_nonnull png);
    
    PNGImage(char* png_nonnull contents,
             long width, long height,
             long numComponents, long bitsPerComponent,
             bool sRGB, float gamma,
             char* png_nullable iccData, long iccDataLength);
    ~PNGImage();
    
    
public:
    [[nodiscard("Don't forget to release the object using PNGImageRelease")]]
    static PNGImage* png_nullable open(const char* png_nonnull path png_noescape) SWIFT_RETURNS_RETAINED SWIFT_NAME(__openUnsafe(path:));
    
    static bool checkIfPNG(const char* png_nonnull path png_noescape);
    
    //std::span<char> getData() lifetimebound SWIFT_COMPUTED_PROPERTY;
    char* png_nonnull getContents() SWIFT_COMPUTED_PROPERTY { return _contents; }
    long getDataSize() const SWIFT_COMPUTED_PROPERTY { return (_bitsPerComponent / 8) * _numComponents * _width * _height; }
    long getWidth() const SWIFT_COMPUTED_PROPERTY { return _width; }
    long getHeight() const SWIFT_COMPUTED_PROPERTY { return _height; }
    long getNumComponents() const SWIFT_COMPUTED_PROPERTY { return _numComponents; }
    long getBitsPerComponent() const SWIFT_COMPUTED_PROPERTY { return _bitsPerComponent; }
    long getBitsPerPixel() const SWIFT_COMPUTED_PROPERTY { return _bitsPerComponent * _numComponents; }
    long getBitsPerRow() const SWIFT_COMPUTED_PROPERTY { return _bitsPerComponent * _numComponents * _width / 8; }
    
    bool getIsSRGB() const SWIFT_COMPUTED_PROPERTY { return _sRGB; }
    float getGamma() const SWIFT_COMPUTED_PROPERTY { return _gamma; }
    
    const char* png_nullable getICCPData() SWIFT_NAME(_getICCPDataUnsafe()) { return _iccData; };
    long getICCPDataLength() SWIFT_NAME(_getICCPDataLengthUnsafe()) { return _iccDataLength; };
}
SWIFT_PRIVATE_FILEID("LibPNG/LibPNG.swift")
SWIFT_SHARED_REFERENCE(PNGImageRetain, PNGImageRelease)
SWIFT_UNCHECKED_SENDABLE;
