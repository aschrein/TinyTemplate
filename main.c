// MIT License
//
// Copyright (c) 2023 Anton Schreiner
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STBI_NO_SIMD
#include "3rdparty/stb/stb_image.h"
#include "3rdparty/stb/stb_image_write.h"

#define u8 uint8_t
#define u32 uint32_t
#define i32 int32_t
#define f32 float
#define u64 uint64_t
#define mkf32(_x) ((float)(_x))
#define mku64(_x) ((uint64_t)(_x))
#define mku32(_x) ((uint32_t)(_x))
#define mki32(_x) ((int32_t)(_x))
#define mku8(_x)  ((uint8_t)(_x))
#define clamp(_x, _min, _max) (((_x) < (_min)) ? (_min) : (((_x) > (_max)) ? (_max) : (_x)))
#define xfor(_N) for (u32 x = mku32(0); x < (_N); x++)
#define yfor(_N) for (u32 y = mku32(0); y < (_N); y++)

typedef struct {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} f32x4;

/**
 * Convert image from RGBA32_FLOAT to RGBA8_UNORM and use stb to write png
 * Allocates width*height*1 scratch using malloc
*/
static void write_f32x4_png(char const *filename, void const *src_data, u64 width, u64 height, u64 pitch) {
    if (pitch == mku64(-1)) pitch = width * sizeof(f32x4);
    u8* data = malloc(width * height * 4);
    yfor(height) {
        xfor(width) {
            f32x4 src                                      = ((f32x4 const *)((u8 *)src_data + pitch * y))[x];
            data[x * mku64(4) + y * width * mku64(4) + mku64(0)] = mku8(clamp(src.x, mkf32(0.0), mkf32(1.0)) * mkf32(255.0));
            data[x * mku64(4) + y * width * mku64(4) + mku64(1)] = mku8(clamp(src.y, mkf32(0.0), mkf32(1.0)) * mkf32(255.0));
            data[x * mku64(4) + y * width * mku64(4) + mku64(2)] = mku8(clamp(src.z, mkf32(0.0), mkf32(1.0)) * mkf32(255.0));
            data[x * mku64(4) + y * width * mku64(4) + mku64(3)] = mku8(255);
        }
    }
    stbi_write_png(filename, mki32(width), mki32(height), STBI_rgb_alpha, &data[0], mki32(width) * mki32(4));
    free(data);
}

int main(int argc, char **argv) {
    fprintf(stdout, "Hello world!\n");

    const u32 size = mku32(512);
    f32x4 *image = malloc(size * size * sizeof(f32x4));
    yfor(size) {
        xfor(size) {
            const f32 u = (mkf32(x) + mkf32(0.5)) / mkf32(size); 
            const f32 v = (mkf32(y) + mkf32(0.5)) / mkf32(size);
            image[x + y * size] = (f32x4){mkf32(fabs(sin(u * mkf32(16.0) * mkf32(3.141592)))),
                                          mkf32(fabs(sin(v * mkf32(16.0) * mkf32(3.141592)))),
                                          mkf32(0.0),
                                          mkf32(1.0)}; 
        }
    }
    write_f32x4_png("image.png", image, /* width */ mku64(size), /* height */ mku64(size), /* pitch */ mku64(size) * sizeof(f32x4));

    free(image);

    // const char c = getc(stdin);
    // fprintf(stdout, "Got char %c\n", c);
    return 0;
}