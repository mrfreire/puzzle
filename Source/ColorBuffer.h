#pragma once

struct ColorBuffer
{
    char* m_color;
    size_t m_width;
    size_t m_height;
    size_t m_colorBufferBytes;
    size_t m_bytesPerPixel;
};
