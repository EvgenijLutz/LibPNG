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
///
/// - Note: This class is immutable, thus thread-safe.
class PNGImage final {
public:
    struct WriteSettings final {
        /// Compression level in range [0 ... 1] - from uncompressed to max compression.
        float compressionLevel;
    };
    
private:
    std::atomic<size_t> _referenceCounter;
    
    /// Pixel buffer, contains uint values.
    const char* fn_nonnull _contents;
    const long _width;
    const long _height;
    /// Number of compoments in a pixel - `1`, `2`, `3` or `4`.
    const long _numComponents;
    /// Bits per component - `8 (uint8)`, `16 (uint16)` or `32 (uint32)`.
    const long _bitsPerComponent;
    
    /// Whether the image uses sRGB gamma correction.
    const bool _sRGB;
    /// Gamma correction value. Not always presented. Greater than `0` if set.
    const float _gamma;
    
    /// Custom ICC profile data
    const char* fn_nullable _iccData;
    const long _iccDataLength;
    
    FN_FRIEND_SWIFT_INTERFACE(PNGImage)
    
    PNGImage(const char* fn_nonnull contents,
             long width, long height,
             long numComponents, long bitsPerComponent,
             bool sRGB, float gamma,
             char* fn_nullable iccData, long iccDataLength);
    ~PNGImage();
    
    struct _LoadInfo {
        bool usePath;
        union {
            const char* fn_nonnull path;
            struct {
                const void* fn_nonnull buffer;
                long bufferSize;
            };
        };
    };
    static bool _checkIfPNG(const _LoadInfo& info);
    static PNGImage* fn_nullable _open(const _LoadInfo& info) SWIFT_RETURNS_RETAINED SWIFT_NAME(__openUnsafe(path:));
    long _write(const char* fn_nullable path fn_noescape, void* fn_nullable * fn_nullable outData fn_noescape, long* fn_nullable outSize fn_noescape, WriteSettings settings);
    
    
public:
    /// Checks if a file at the specified `path` is a `png` image.
    static bool checkIfPNG(const char* fn_nonnull path fn_noescape);
    static bool checkIfPNG(const void* fn_nonnull buffer fn_noescape, long bufferSize);
    
    
    static PNGImage* fn_nonnull create(const char* fn_nonnull contents fn_noescape,
                                       long width, long height,
                                       long numComponents, long bitsPerComponent,
                                       bool sRGB = true, float gamma = -1,  // Default to sRGB
                                       const char* fn_nullable iccData fn_noescape fn_counted_by(iccDataLength) = nullptr, long iccDataLength = 0    // Default to no colour profile
                                       ) SWIFT_RETURNS_RETAINED SWIFT_NAME(init(_:_:_:_:_:_:_:_:_:));
    
    /// Loads a png file at the specified `path`.
    ///
    /// - Returns: a valid ``PNGImage`` if the image was successfully loaded, otherwise false.
    [[nodiscard("Don't forget to release the object using the PNGImageRelease function")]]
    static PNGImage* fn_nullable open(const char* fn_nonnull path fn_noescape) SWIFT_RETURNS_RETAINED SWIFT_NAME(__openUnsafe(path:));
    static PNGImage* fn_nullable open(const void* fn_nonnull buffer fn_noescape fn_counted_by(bufferSize), long bufferSize) SWIFT_RETURNS_RETAINED SWIFT_NAME(__openUnsafe(buffer:size:));
    
    /// Writes a `png` compressed image into a buffer.
    /// - Parameter outData: pointer to output compressed data contents.
    /// - Parameter outSize: pointer to output compressed data size.
    /// - Seealso: [Writing PNG Images](https://www.libpng.org/pub/png/book/chapter15.html)
    /// - Returns: `0` if succeeded, otherwise an error code.
    long write(void* fn_nullable * fn_nonnull outData fn_noescape, long* fn_nonnull outSize fn_noescape, WriteSettings settings);
    /// Writes a `png` compressed image into a file.
    /// - Parameter path: path to the output png image. If a file at the specified path exits, it will be overwritten.
    /// - Seealso: [Writing PNG Images](https://www.libpng.org/pub/png/book/chapter15.html)
    /// - Returns: `0` if succeeded, otherwise an error code.
    long write(const char* fn_nonnull path fn_noescape, WriteSettings settings) SWIFT_NAME(__writeUnsafe(_:_:));
    
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
