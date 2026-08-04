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


PNGImage* fn_nullable PNGImage::open(const char* fn_nonnull path fn_noescape) SWIFT_RETURNS_RETAINED {
    return _open({
        .usePath = true,
        .path = path
    });
}


PNGImage* fn_nullable PNGImage::open(const void* fn_nonnull buffer fn_noescape, long bufferSize) SWIFT_RETURNS_RETAINED {
    return _open({
        .usePath = false,
        .buffer = buffer,
        .bufferSize = bufferSize
    });
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


FN_IMPLEMENT_SWIFT_INTERFACE1(PNGImage)
