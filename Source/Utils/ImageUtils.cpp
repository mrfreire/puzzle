#include "ImageUtils.h"

#include <stdint.h>

void ImageUtils::StbToWin32Bitmap(char* img, int w, int h)
{
    const int count = w * h;
    uint32_t* p = (uint32_t*)img;

    for (int i = 0; i < count; ++i)
    {
        uint32_t newColour = 0;
        newColour |= (*p << 16) & 0x00ff0000;
        newColour |= (*p >> 16) & 0x000000ff;
        newColour |= *p & 0xff00ff00;
        *p++ = newColour;
    }
}
