#ifndef __TGA_H__
#define __TGA_H__

#include "util.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>

/**
 * <http://paulbourke.net/dataformats/tga/>
 */
struct tga_t {
    struct {
        uint16_t origin = 0;
        uint16_t length = 0;
        uint8_t entrySize = 0;
    } colorMapSpec;

    struct {
        uint16_t originX = 0;
        uint16_t originY = 0;
        uint16_t width = 0;
        uint16_t height = 0;
        uint8_t pixelSize = 0;
        uint8_t descriptor = 0;
    } imageMapSpec;

    color_t* imageData = nullptr;

    void read(uint8_t* dst, FILE* file) {
        fread(dst, 1, 1, file);
    }

    void read(uint16_t* dst, FILE* file) {
        uint8_t bytes[2];
        fread(bytes, 2, 1, file);
        *dst = bytes[0] + (bytes[1] << 8);
    }

    int read(const char* filename) {
        auto file = fopen(filename, "rb");

        uint8_t imageIdLen = 0;
        fread(&imageIdLen, 1, 1, file);

        uint8_t colorMapType = 0;
        fread(&colorMapType, 1, 1, file);

        uint8_t imageTypeCode = 0;
        fread(&imageTypeCode, 1, 1, file);
        printf("imageTypeCode = 0x%02X\n", imageTypeCode);

        read(&colorMapSpec.origin, file);
        read(&colorMapSpec.length, file);
        read(&colorMapSpec.entrySize, file);

        read(&imageMapSpec.originX, file);
        read(&imageMapSpec.originY, file);
        read(&imageMapSpec.width, file);
        read(&imageMapSpec.height, file);
        read(&imageMapSpec.pixelSize, file);
        read(&imageMapSpec.descriptor, file);

        if(imageMapSpec.pixelSize != 24 && imageMapSpec.pixelSize != 32) {
            printf("F [tga.read] pixelSize = %d\n", imageMapSpec.pixelSize);
            exit(1);
        }

        imageData = new color_t[imageMapSpec.width * imageMapSpec.height];

        char* imageId = new char[imageIdLen];
        fread(imageId, imageIdLen, 1, file);

        // colorMapData
        fseek(file, colorMapSpec.length * colorMapSpec.entrySize / 8, SEEK_CUR);

        if(imageTypeCode == 0x0A) { // Run Length Encoded, RGB images
            for(int i = 0; i < imageMapSpec.width * imageMapSpec.height;) {
                uint8_t header = 0;
                fread(&header, 1, 1, file);
                int length = (header & 0x7F) + 1;

                uint8_t bgra[4] = { 0, 0, 0, 255 };
                for(int j = 0; j < length; j++, i++) {
                    // read once if RLE
                    if((header & 0x80) == 0 || j == 0) fread(bgra, imageMapSpec.pixelSize / 8, 1, file);
                    imageData[i].r = bgra[2];
                    imageData[i].g = bgra[1];
                    imageData[i].b = bgra[0];
                    imageData[i].a = bgra[3];
                }
            }
        }

        fclose(file);
        return 0;
    }
};

#endif // __TGA_H__
