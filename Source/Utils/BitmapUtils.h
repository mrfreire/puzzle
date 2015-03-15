#pragma once

namespace BitmapUtils
{
    // Allocates a buffer and loads the contents of path in it
    unsigned char* LoadBitmapFile(const char* path);
}
