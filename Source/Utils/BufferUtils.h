#pragma once

namespace BufferUtils
{
    char* GetPointerToElement(const char* a, int colsCount, int bytesPerElement, int x, int y)
    {
        int index = y * colsCount + x;
        int offset = index * bytesPerElement;
        return (char*)&a[offset];
    }
}
