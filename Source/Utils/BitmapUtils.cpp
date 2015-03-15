#include "BitmapUtils.h"

#include "Log.h"

#include "../External/pow2assert.h"

#include <windows.h>

unsigned char* BitmapUtils::LoadBitmapFile(const char* path)
{
    POW2_ASSERT(path);

    HANDLE hnd = LoadImageA(NULL, path, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);

    if (!hnd)
    {
        Log::Error("The bitmap file %s couldn't be loaded", path);
    }

    return hnd;
}
