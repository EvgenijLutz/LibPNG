//
//  LibPNGC.hpp
//  LibPNGFramework
//
//  Created by Evgenij Lutz on 06.08.25.
//

#ifndef LibPNGC_hpp
#define LibPNGC_hpp

#if defined __cplusplus

#include <stdio.h>
#include <atomic>
#include <swift/bridging>


#ifndef nullable
#define nullable __nullable
#endif

#ifndef nonnull
#define nonnull __nonnull
#endif


class GlamorousPNG {
private:
    std::atomic<size_t> referenceCounter;
    
    // Prevent exposing libpng api
    // Client code will recognize png_struct and png_info as void
#if !defined PNG_H
#define png_struct void
#define png_info void
#endif
    
    png_struct* nonnull png;
    png_info* nonnull startInfo;
    png_info* nonnull endInfo;
    
    
    /// Temporary data.
    char* nonnull _data;
    long _width;
    long _height;
    long _numComponents;
    long _bitsPerComponent;
    
    
    friend GlamorousPNG* nonnull GlamorousPNGRetain(GlamorousPNG* nonnull png) SWIFT_RETURNS_RETAINED;
    friend void GlamorousPNGRelease(GlamorousPNG* nonnull png);
    friend GlamorousPNG* nullable openGlamorousPNG(const char* nonnull path) SWIFT_RETURNS_RETAINED;
    
    GlamorousPNG(png_struct* nonnull png, png_info* nonnull startInfo, png_info* nonnull endInfo,
                 char* nonnull data, long width, long height, long numComponents, long bitsPerComponent);
    ~GlamorousPNG();
    
#if !defined PNG_H
#undef png_struct
#undef png_info
#endif
    
public:
    char* nonnull getData() SWIFT_RETURNS_INDEPENDENT_VALUE SWIFT_COMPUTED_PROPERTY { return _data; }
    long getDataSize() const SWIFT_COMPUTED_PROPERTY { return (_bitsPerComponent / 8) * _numComponents * _width * _height; }
    long getWidth() const SWIFT_COMPUTED_PROPERTY { return _width; }
    long getHeight() const SWIFT_COMPUTED_PROPERTY { return _height; }
    long getNumComponents() const SWIFT_COMPUTED_PROPERTY { return _numComponents; }
    long getBitsPerComponent() const SWIFT_COMPUTED_PROPERTY { return _bitsPerComponent; }
    long getBitsPerPixel() const SWIFT_COMPUTED_PROPERTY { return _bitsPerComponent * _numComponents; }
    long getBitsPerRow() const SWIFT_COMPUTED_PROPERTY { return _bitsPerComponent * _numComponents * _width / 8; }
    
} SWIFT_SHARED_REFERENCE(GlamorousPNGRetain, GlamorousPNGRelease);

GlamorousPNG* nonnull GlamorousPNGRetain(GlamorousPNG* nonnull png) SWIFT_RETURNS_RETAINED;
void GlamorousPNGRelease(GlamorousPNG* nonnull png);

[[nodiscard("Don't forget to release the object using GlamorousPNGRelease, darling")]]
GlamorousPNG* nullable openGlamorousPNG(const char* nonnull path) SWIFT_RETURNS_RETAINED;

#endif // __cplusplus

#endif // LibPNGC_hpp
