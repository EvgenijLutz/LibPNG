//
//  PNGImage.cpp
//  LibPNG
//
//  Created by Evgenij Lutz on 31.10.25.
//

#include <LibPNGC/PNGImage.hpp>
#include <png.h>
#include <LibPNGC/LibPNGC.hpp>
#include <stdio.h>
#include <iostream>


PNGImage::PNGImage(const char* fn_nonnull contents,
                   long width, long height,
                   long numComponents, long bitsPerComponent,
                   bool sRGB, float gamma,
                   char* fn_nullable iccData, long iccDataLength):
_referenceCounter(1),
_contents(contents),
_width(width),
_height(height),
_numComponents(numComponents),
_bitsPerComponent(bitsPerComponent),
_sRGB(sRGB),
_gamma(gamma),
_iccData(iccData),
_iccDataLength(iccDataLength) {
    printf("Glamorous hieee\n");
}


PNGImage::~PNGImage() {
    printf("Glamorous byeee\n");
    
    if (_iccData) {
        delete [] _iccData;
    }
    
    if (_contents) {
        delete [] _contents;
    }
}


//std::span<char> PNGImage::getData() {
//    return std::span(_data, (_bitsPerComponent / 8) * _numComponents * _width * _height);
//}


void pngErrorCallback(png_structp, png_const_charp) {
    
}

void pngWarningCallback(png_structp, png_const_charp) {
    
}


int read_chunk_callback(png_struct* ptr, png_unknown_chunkp chunk) {
    //auto userInfo = png_get_user_chunk_ptr(ptr);
    printf("Chunk: %s\n", chunk->name);
    
    // TODO: PNG CgBI Format
    // https://theapplewiki.com/wiki/PNG_CgBI_Format
    
    /* The unknown chunk structure contains your
     chunk data, along with similar data for any other
     unknown chunks: */
    /* png_byte name[5];
    png_byte *data;
    png_size_t size; */
    /* Note that libpng has already taken care of
     the CRC handling */
    /* put your code here. Search for your chunk in the
     unknown chunk structure, process it, and return one
     of the following: */
    //return (-1); /* chunk had an error */
    return (0); /* did not recognize */
    //return (1); /* success */
}


void read_row_callback(png_struct* ptr, png_uint_32 row, int pass) {
    // update progress meter
}

struct BufferReader {
    bool usePath = false;
    
    FILE* file = nullptr;
    
    const void* data = nullptr;
    long size = 0;
    long offset = 0;
    
    void close() {
        if (usePath && file) {
            std::fclose(file);
            file = nullptr;
        }
    }
    
    bool read(void* dst, long numBytes) {
        if (usePath) {
            std::fread(dst, numBytes, 1, file);
        }
        else {
            const png_byte* png_src = reinterpret_cast<const png_byte*>(data) + offset;
            std::memcpy(dst, png_src, numBytes);
            offset += numBytes;
        }
        
        return true;
    }
    
    static void ReadDataFromInputStream(png_structp png_ptr, png_byte* raw_data, png_size_t read_length) {
        BufferReader* handle = (BufferReader*)png_get_io_ptr(png_ptr);
        handle->read(raw_data, read_length);
    }
    
    
    bool initWithPath(const char* fn_nonnull path) {
        usePath = true;
        
        // Read the header of the file to check if it's really a png file
        file = std::fopen(path, "rb");
        if (file == nullptr) {
            return false;
        }
        
        return true;
    }
    
    void initWithBuffer(const void* fn_nonnull buffer, long bufferSize) {
        usePath = false;
        data = buffer;
        size = bufferSize;
    }
};


PNGImage* fn_nullable PNGImage::_open(const _LoadInfo& info) {
    auto reader = BufferReader();
    if (info.usePath) {
        if (reader.initWithPath(info.path) == false) {
            return nullptr;
        }
    }
    else {
        reader.initWithBuffer(info.buffer, info.bufferSize);
    }
    
    // Read the header of the file to check if it's really a png file
    // Compare PNG signature
    png_byte header[8];
    auto headerSize = static_cast<int>(sizeof(header));
    reader.read(header, headerSize);
    if (png_sig_cmp(header, 0, headerSize)) {
        printf("Wrong PNG signature\n");
        return nullptr;
    }
    
    // Allocate memory for PNG
    png_voidp pngError = nullptr;
    png_struct* png = png_create_read_struct(PNG_LIBPNG_VER_STRING, pngError, pngErrorCallback, pngWarningCallback);
    if (png == nullptr) {
        printf("Could not create png_struct\n");
        return nullptr;
    }
    
    // PNG information
    png_info* startInfo = png_create_info_struct(png);
    if (startInfo == nullptr) {
        printf("Could not create start png_info\n");
        png_destroy_read_struct(&png, nullptr, nullptr);
        return nullptr;
    }
    
    // PNG end information
    png_info* endInfo = png_create_info_struct(png);
    if (endInfo == nullptr) {
        printf("Could not create end png_info\n");
        png_destroy_read_struct(&png, &startInfo, nullptr);
        return nullptr;
    }
    
    
    // When libpng encounters later an error, this will be called:
    if (setjmp(png_jmpbuf(png))) {
        printf("Encountered an error, abort\n");
        png_destroy_read_struct(&png, &startInfo, &endInfo);
        reader.close();
        return nullptr;
    }
    
    
    // Setup png reading
    if (info.usePath) {
        png_init_io(png, reader.file);
    }
    else {
        png_set_read_fn(png, &reader, BufferReader::ReadDataFromInputStream);
    }
    
    
    // We've read the header previously
    png_set_sig_bytes(png, headerSize);
    
    // Set unknown chunk callback
    png_set_read_user_chunk_fn(png, nullptr, read_chunk_callback);
    
    // Set read callback, usefull for progress bars
    png_set_read_status_fn(png, read_row_callback);
    
    // Discard all unknown chunks
    png_set_keep_unknown_chunks(png, PNG_HANDLE_CHUNK_NEVER, nullptr, 0);
    
    // Read the whole png file into memory
    png_read_png(png, startInfo, PNG_TRANSFORM_IDENTITY, nullptr);
    
    
    // Close the reader
    reader.close();
    
    
    // Get png data
    auto colorType = (int)png_get_color_type(png, startInfo);
    auto bitDepth = (int)png_get_bit_depth(png, startInfo);
    
    // Convert color palette to RGB
    // https://www.libpng.org/pub/png/libpng-1.2.5-manual.html
    if (colorType == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png);
        colorType = (int)png_get_color_type(png, startInfo);
    }
    
    // 1, 2, 4 bit gray to 8 bit gray
    if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8) {
        png_set_expand_gray_1_2_4_to_8(png);
        bitDepth = (int)png_get_bit_depth(png, startInfo);
    }
    
    // Embed transparency into image data if there is a tRNS block
    if (png_get_valid(png, startInfo, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png);
        colorType = (int)png_get_color_type(png, startInfo);
    }
        
    // Check if image is explicitly marked as sRGB
    int sRGBIntent = 0;
    auto isSRGB = png_get_sRGB(png, startInfo, &sRGBIntent) == PNG_INFO_sRGB;
    if (isSRGB) {
        printf("Image is sRGB. Intent: %d\n", sRGBIntent);
    }
    else {
        printf("Image does not contain sRGB block\n");
    }
    
    // Check if there is an ICC profile
    char* iccData = nullptr;
    long iccDataLength = 0;
    if (isSRGB == false) {
        png_charp name;
        int compression_type;
        png_bytep profile_data;
        png_uint_32 profile_len;
        auto result = png_get_iCCP(png, startInfo, &name, &compression_type, &profile_data, &profile_len);
        if (result == PNG_INFO_iCCP) {
            if (compression_type == PNG_COMPRESSION_TYPE_BASE) {
                if (name) {
                    printf("Color space: %s\n", name);
                }
                iccData = new char[profile_len];
                iccDataLength = static_cast<long>(profile_len);
                memcpy(iccData, profile_data, profile_len);
            } else {
                printf("Unsupported ICC compression type %d\n", compression_type);
            }
        } else {
            printf("Could not get iCCP chunk\n");
        }
    }
    
    // Check gamma
    double gamma = -1.0;
    auto isGamma = png_get_gAMA(png, startInfo, &gamma) == PNG_INFO_gAMA;
    if (isGamma) {
        printf("Gamma: %f\n", gamma);
        if (fabs(gamma - 1.0) < 0.001) {
            printf("Likely linear\n");
        }
        else if (fabs(gamma - 0.454545455) < 0.001) {
            printf("Likely gamma corrected sRGB\n");
        }
    }
    
    // Determine number of color components
    auto numChannels = (int)png_get_channels(png, startInfo);
    
    auto width = png_get_image_width(png, startInfo);
    auto height = png_get_image_height(png, startInfo);
    auto rowPointers = png_get_rows(png, startInfo);
    auto rowSize = numChannels * bitDepth * width / 8;
    auto imageSize = static_cast<long>(rowSize * height);
    auto data = new char[imageSize];
    
    for (auto rowIndex = 0; rowIndex < height; rowIndex++) {
        auto row = rowPointers[rowIndex];
        memcpy(data + rowSize * rowIndex, row, rowSize);
    }
    
    png_destroy_read_struct(&png, &startInfo, &endInfo);
    
    return new PNGImage(data, width, height, numChannels, bitDepth, isSRGB, static_cast<float>(gamma), iccData, iccDataLength);
}


long PNGImage::_write(const char* fn_nullable path fn_noescape, void* fn_nullable * fn_nullable outData fn_noescape, long* fn_nullable outSize fn_noescape, WriteSettings settings) {
    int colorType;
    switch (_numComponents) {
        case 1: colorType = PNG_COLOR_TYPE_GRAY; break;
        case 2: colorType = PNG_COLOR_TYPE_GRAY_ALPHA; break;
        case 3: colorType = PNG_COLOR_TYPE_RGB; break;
        case 4: colorType = PNG_COLOR_TYPE_RGB_ALPHA; break;
        default:
            return 3;
    }
    
    if (_width <= 0 || _height <= 0) {
        return 3;
    }
    
    if (_bitsPerComponent != 8 && _bitsPerComponent != 16) {
        return 3;
    }
    
    struct _writer {
        const char* fn_nonnull path;
        FILE* file;
        
        void* fn_nullable * fn_nullable outData;
        long* fn_nullable outSize;
        png_bytep memory;
        png_size_t memorySize;
        png_size_t memoryCapacity;
        bool releaseMemoryOnCleanup;
        
        
        ~_writer() {
            cleanup();
        }
        
        
        void cleanup() {
            if (file) {
                std::fclose(file);
                file = nullptr;
            }
            
            if (releaseMemoryOnCleanup && memory) {
                delete [] memory;
                memory = nullptr;
            }
        }
        
        
        bool append(const png_bytep data, png_size_t size) {
            if (size == 0) {
                return true;
            }
            
            if (memorySize > PNG_SIZE_MAX - size) {
                return false;
            }
            
            auto requiredSize = memorySize + size;
            if (requiredSize > memoryCapacity) {
                auto newCapacity = memoryCapacity == 0 ? static_cast<png_size_t>(4096) : memoryCapacity;
                while (newCapacity < requiredSize) {
                    if (newCapacity > PNG_SIZE_MAX / 2) {
                        newCapacity = requiredSize;
                        break;
                    }
                    newCapacity *= 2;
                }
                
                auto newMemory = new png_byte[newCapacity];
                if (memory) {
                    std::memcpy(newMemory, memory, memorySize);
                    delete [] memory;
                }
                memory = newMemory;
                memoryCapacity = newCapacity;
            }
            
            std::memcpy(memory + memorySize, data, size);
            memorySize = requiredSize;
            return true;
        }
        
        
        static void pngErrorCallback(png_structp, png_const_charp) {
            //
        }
        
        static void pngWarningCallback(png_structp, png_const_charp) {
            //
        }
        
        static void writeData(png_structp png_ptr, png_bytep data, size_t size) {
            auto writer = reinterpret_cast<_writer*>(png_get_io_ptr(png_ptr));
            if (writer->append(data, size) == false) {
                png_error(png_ptr, "Could not allocate PNG output buffer");
            }
        }
        
        static void outputFlush(png_structp) {
            // No buffered state to flush for memory output.
        }
    };
    auto usePath = path != nullptr;
    auto writer = _writer {
        .path = path,
        .file = nullptr,
        .outData = outData,
        .outSize = outSize,
        .memory = nullptr,
        .memorySize = 0,
        .memoryCapacity = 0,
        .releaseMemoryOnCleanup = true
    };
    
    if (usePath) {
        writer.file = std::fopen(path, "wb");
        if (writer.file == nullptr) {
            return 1;
        }
    }
    
    png_voidp pngError = nullptr;
    //png_create_write_struct_2(PNG_LIBPNG_VER_STRING, pngError, _writer::pngErrorCallback, _writer::pngWarningCallback, nullptr, malloc, free);
    auto png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, pngError, _writer::pngErrorCallback, _writer::pngWarningCallback);
    if (!png_ptr) {
        return 4;
    }
    
    auto info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, NULL);
        return 4;
    }
    
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 4;
    }
    
    if (usePath) {
        png_init_io(png_ptr, writer.file);
    }
    else {
        if (outData == nullptr || outSize == nullptr) {
            png_destroy_write_struct(&png_ptr, &info_ptr);
            return 2;
        }
        
        *outData = nullptr;
        *outSize = 0;
        png_set_write_fn(png_ptr, &writer, _writer::writeData, _writer::outputFlush);
    }
    
    constexpr float highestCompressionLevel = 9;
    auto compressionLevel = static_cast<int>(settings.compressionLevel * highestCompressionLevel);
    if (compressionLevel < 0) {
        compressionLevel = 0;
    }
    else if (compressionLevel > 9) {
        compressionLevel = 9;
    }
    png_set_compression_level(png_ptr, compressionLevel);
    
    png_set_IHDR(png_ptr,
                 info_ptr,
                 static_cast<png_uint_32>(_width),
                 static_cast<png_uint_32>(_height),
                 static_cast<int>(_bitsPerComponent),
                 colorType,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);
    
    if (_sRGB) {
        png_set_sRGB(png_ptr, info_ptr, PNG_sRGB_INTENT_PERCEPTUAL);
    }
    else if (_iccData && _iccDataLength > 0) {
        png_set_iCCP(png_ptr,
                     info_ptr,
                     static_cast<png_const_charp>("ICC Profile"),
                     PNG_COMPRESSION_TYPE_BASE,
                     reinterpret_cast<png_const_bytep>(_iccData),
                     static_cast<png_uint_32>(_iccDataLength));
    }
    else if (_gamma > 0) {
        png_set_gAMA(png_ptr, info_ptr, static_cast<double>(_gamma));
    }
    
    png_write_info(png_ptr, info_ptr);
    
    auto rowSize = static_cast<png_size_t>(getBitsPerRow());
    for (long rowIndex = 0; rowIndex < _height; rowIndex++) {
        auto row = reinterpret_cast<png_const_bytep>(_contents + rowSize * rowIndex);
        png_write_row(png_ptr, row);
    }
    
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    
    if (!usePath) {
        *outData = writer.memory;
        *outSize = static_cast<long>(writer.memorySize);
        writer.memory = nullptr;
        writer.memorySize = 0;
        writer.memoryCapacity = 0;
        writer.releaseMemoryOnCleanup = false;
    }
    
    return 0;
}


PNGImage* fn_nullable PNGImage::open(const char* fn_nonnull path fn_noescape) SWIFT_RETURNS_RETAINED {
    return _open({
        .usePath = true,
        .path = path
    });
}


PNGImage* fn_nullable PNGImage::open(const void* fn_nonnull buffer fn_noescape fn_counted_by(bufferSize), long bufferSize) SWIFT_RETURNS_RETAINED {
    return _open({
        .usePath = false,
        .buffer = buffer,
        .bufferSize = bufferSize
    });
}


long PNGImage::write(void* fn_nullable * fn_nonnull outData fn_noescape, long* fn_nonnull outSize fn_noescape, WriteSettings settings) {
    return _write(nullptr, outData, outSize, settings);
}


long PNGImage::write(const char* fn_nonnull path fn_noescape, WriteSettings settings) {
    return _write(path, nullptr, nullptr, settings);
}


bool PNGImage::_checkIfPNG(const _LoadInfo& info) {
    auto reader = BufferReader();
    if (info.usePath) {
        if (reader.initWithPath(info.path) == false) {
            return false;
        }
    }
    else {
        reader.initWithBuffer(info.buffer, info.bufferSize);
    }
    
    // Compare PNG signature
    png_byte header[8];
    auto headerSize = static_cast<int>(sizeof(header));
    reader.read(header, headerSize);
    if (png_sig_cmp(header, 0, headerSize)) {
        reader.close();
        return false;
    }
    
    // Close the file
    reader.close();
    
    // It's a png file
    return true;
}


bool PNGImage::checkIfPNG(const char* fn_nonnull path fn_noescape) {
    // Sanity check
    if (path == nullptr) {
        printf("Could not check a file, since no path was specified\n");
        return false;
    }
    
    return _checkIfPNG({
        .usePath = true,
        .path = path
    });
}


bool PNGImage::checkIfPNG(const void* fn_nonnull buffer fn_noescape, long bufferSize) {
    return _checkIfPNG({
        .usePath = false,
        .buffer = buffer,
        .bufferSize = bufferSize
    });
}


PNGImage* fn_nonnull PNGImage::create(const char* fn_nonnull contents fn_noescape,
                                      long width, long height,
                                      long numComponents, long bitsPerComponent,
                                      bool sRGB, float gamma,
                                      const char* fn_nullable iccData fn_noescape, long iccDataLength) {
    auto bytesPerComponent = bitsPerComponent / 8;
    auto contentsSize = width * height * numComponents * bytesPerComponent;
    auto contentsCopy = new char[contentsSize];
    std::memcpy(contentsCopy, contents, contentsSize);
    
    char* iccDataCopy = nullptr;
    if (iccData) {
        iccDataCopy = new char[iccDataLength];
        std::memcpy(iccDataCopy, iccData, iccDataLength);
    }
    
    return new PNGImage(contentsCopy,
                        width, height,
                        numComponents, bitsPerComponent,
                        sRGB, gamma,
                        iccDataCopy, iccDataLength);
}


FN_IMPLEMENT_SWIFT_INTERFACE1(PNGImage)
