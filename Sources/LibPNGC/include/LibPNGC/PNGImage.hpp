//
//  PNGImage.hpp
//  LibPNG
//
//  Created by Evgenij Lutz on 31.10.25.
//

#pragma once

#include <LibPNGC/Common.hpp>


enum class PNGImageError: long {
    
};


/// Glamorously decompressed PNG image.
///
/// "It takes a lot of money to look this cheap, darling."
///
/// - Note: This class is immutable, thus thread-safe.
class PNGImage final {
private:
    std::atomic<size_t> _referenceCounter;
    
    /// Pixel buffer, contains uint values.
    std::byte* fn_nonnull _contents;
    bool _ownsContents;
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
    
    /// Custom ICC profile data.
    void* fn_nullable _iccData;
    bool _ownsICCData;
    const long _iccDataLength;
    
    FN_FRIEND_SWIFT_INTERFACE(PNGImage)
    
    PNGImage(std::byte* fn_nonnull contents, bool ownsContents,
             long width, long height,
             long numComponents, long bitsPerComponent,
             bool sRGB, float gamma,
             void* fn_nullable iccData, bool ownsICCData, long iccDataLength);
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
    long _write(const char* fn_nullable path fn_noescape, void* fn_nullable * fn_nullable outData fn_noescape, long* fn_nullable outSize fn_noescape, float compressionLevel);
    
    
public:
    /// Checks if a file at the specified `path` is a `png` image.
    static bool checkIfPNG(const char* fn_nonnull path fn_noescape);
    /// Checks if the specified `buffer` is a `png` image.
    static bool checkIfPNG(const void* fn_nonnull buffer fn_noescape fn_counted_by(bufferSize), long bufferSize);
    
    
    /// Creates a png image from a memory buffer copy.
    static PNGImage* fn_nonnull create(const void* fn_nonnull contents fn_noescape,
                                       long width, long height,
                                       long numComponents, long bitsPerComponent,
                                       bool sRGB = true, float gamma = -1,  // Default to sRGB
                                       const void* fn_nullable iccData fn_noescape fn_counted_by(iccDataLength) = nullptr, long iccDataLength = 0    // Default to no colour profile
                                       ) SWIFT_RETURNS_RETAINED SWIFT_NAME(init(_:_:_:_:_:_:_:_:_:));
    
    /// Creates a png image by referencing memory buffer for its contents and ICC profile (if specified) without copying them.
    ///
    /// - Parameter ownsContents: `true` if the resulting image should take ownership over the `contents` and release it using the `std::free` function when not used anymore. Otherwise make sure that the the `contents` buffer stays alive and unchanged during the whole lifetime of the returned ``PNGImage`` object.
    /// - Parameter ownsICCData: `true` if the resulting image should take ownership over the `iccData` and release it using the `std::free` function when not used anymore. Otherwise make sure that the the `iccData` buffer stays alive and unchanged during the whole lifetime of the returned ``PNGImage`` object.
    static PNGImage* fn_nonnull createTransferring(void* fn_nonnull contents fn_noescape, bool ownsContents,
                                                   long width, long height,
                                                   long numComponents, long bitsPerComponent,
                                                   bool sRGB = true, float gamma = -1,  // Default to sRGB
                                                   void* fn_nullable iccData fn_noescape fn_counted_by(iccDataLength) = nullptr, bool ownsICCData = true, long iccDataLength = 0    // Default to no colour profile
    ) SWIFT_RETURNS_RETAINED SWIFT_NAME(__createUnownedUnsafe(_:_:_:_:_:_:_:_:_:_:_:));
    
    /// Loads a png file at the specified `path`.
    ///
    /// - Returns: a valid ``PNGImage`` if the image was successfully loaded, otherwise false.
    [[nodiscard("Don't forget to release the object using the PNGImageRelease function")]]
    static PNGImage* fn_nullable open(const char* fn_nonnull path fn_noescape) SWIFT_RETURNS_RETAINED SWIFT_NAME(__openUnsafe(path:));
    
    /// Loads a png file from a memory buffer.
    ///
    /// - Returns: a valid ``PNGImage`` if the image was successfully loaded, otherwise false.
    [[nodiscard("Don't forget to release the object using the PNGImageRelease function")]]
    static PNGImage* fn_nullable open(const void* fn_nonnull buffer fn_noescape fn_counted_by(bufferSize), long bufferSize) SWIFT_RETURNS_RETAINED SWIFT_NAME(__openUnsafe(buffer:size:));
    
    /// Writes a `png` compressed image into a buffer.
    /// - Parameter outData: pointer to output compressed data contents.
    /// - Parameter outSize: pointer to output compressed data size.
    /// - Parameter compressionLevel: compression level in range [0 ... 1] - from uncompressed to max compression.
    /// - Seealso: [Writing PNG Images](https://www.libpng.org/pub/png/book/chapter15.html)
    /// - Returns: `0` if succeeded, otherwise an error code.
    long write(void* fn_nullable * fn_nonnull outData fn_noescape, long* fn_nonnull outSize fn_noescape, float compressionLevel);
    /// Writes a `png` compressed image into a file.
    /// - Parameter path: path to the output png image. If a file at the specified path exits, it will be overwritten.
    /// - Parameter compressionLevel: compression level in range [0 ... 1] - from uncompressed to max compression.
    /// - Seealso: [Writing PNG Images](https://www.libpng.org/pub/png/book/chapter15.html)
    /// - Returns: `0` if succeeded, otherwise an error code.
    long write(const char* fn_nonnull path fn_noescape, float compressionLevel) SWIFT_NAME(__writeUnsafe(_:_:));
    
    //std::span<std::byte> getData() lifetimebound SWIFT_COMPUTED_PROPERTY;
    const void* fn_nonnull getContents() fn_lifetimebound SWIFT_COMPUTED_PROPERTY { return _contents; }
    long getDataSize() const SWIFT_COMPUTED_PROPERTY { return (_bitsPerComponent / 8) * _numComponents * _width * _height; }
    long getWidth() const SWIFT_COMPUTED_PROPERTY { return _width; }
    long getHeight() const SWIFT_COMPUTED_PROPERTY { return _height; }
    long getNumComponents() const SWIFT_COMPUTED_PROPERTY { return _numComponents; }
    long getBitsPerComponent() const SWIFT_COMPUTED_PROPERTY { return _bitsPerComponent; }
    long getBitsPerPixel() const SWIFT_COMPUTED_PROPERTY { return _bitsPerComponent * _numComponents; }
    long getBytesPerRow() const SWIFT_COMPUTED_PROPERTY { return (_bitsPerComponent / 8) * _numComponents * _width; }
    
    bool getIsSRGB() const SWIFT_COMPUTED_PROPERTY { return _sRGB; }
    float getGamma() const SWIFT_COMPUTED_PROPERTY { return _gamma; }
    
    const void* fn_nullable getICCPData() fn_lifetimebound SWIFT_NAME(__getICCPDataUnsafe()) { return _iccData; };
    long getICCPDataLength() SWIFT_NAME(_getICCPDataLengthUnsafe()) { return _iccDataLength; };
    
    struct Unsafe final {
        Unsafe() = delete;
        static void* fn_nullable transferContentsOwnership(PNGImage* fn_nonnull png fn_noescape);
        static void* fn_nullable transferICCDataOwnership(PNGImage* fn_nonnull png fn_noescape);
    };
}
SWIFT_PRIVATE_FILEID("LibPNG/LibPNG.swift")
FN_SWIFT_INTERFACE(PNGImage)
SWIFT_UNCHECKED_SENDABLE;


FN_DEFINE_SWIFT_INTERFACE(PNGImage)
