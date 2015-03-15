#include "ColorBuffer.h"
#include "Puzzle.h"
#include "Utils/DebugTimer.h"
#include "Utils/ImageUtils.h"
#include "Utils/Log.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "External/stb_image.h"

#include <stdint.h>
#include <stdio.h>
#include <windows.h>

struct AppState  // zero is initialisation
{
    BITMAPINFO m_bitmapInfo;
    ColorBuffer m_colorBuffer;
    bool m_quit;
};

int g_bitmapHeight;
int g_bitmapWidth;
int g_bitmapBytes;

static AppState g_app;

static void ResizeBitmap(int width, int height)
{
    g_bitmapWidth = width;
    g_bitmapHeight = height;

    g_app.m_bitmapInfo.bmiHeader.biSize = sizeof(g_app.m_bitmapInfo.bmiHeader);
    g_app.m_bitmapInfo.bmiHeader.biWidth = width;
    g_app.m_bitmapInfo.bmiHeader.biHeight = -height;  // top-down
    g_app.m_bitmapInfo.bmiHeader.biPlanes = 1;
    g_app.m_bitmapInfo.bmiHeader.biBitCount = 32;
    g_app.m_bitmapInfo.bmiHeader.biCompression = BI_RGB;

    //
    // Destroy buffers
    //

    if (g_app.m_colorBuffer.m_color)
    {
        VirtualFree(g_app.m_colorBuffer.m_color, 0, MEM_RELEASE);
    }

    //
    // Create new buffers
    //

    g_app.m_colorBuffer.m_bytesPerPixel = 4;
    g_app.m_colorBuffer.m_colorBufferBytes = g_app.m_colorBuffer.m_bytesPerPixel * width * height;
    g_app.m_colorBuffer.m_color = (char*)VirtualAlloc(
        0, g_app.m_colorBuffer.m_colorBufferBytes, MEM_COMMIT, PAGE_READWRITE);
    g_bitmapBytes = g_app.m_colorBuffer.m_colorBufferBytes;
    
    g_app.m_colorBuffer.m_width = width;
    g_app.m_colorBuffer.m_height = height;
}

static void DrawBitmap(HDC hdc, RECT clientRect)
{
    StretchDIBits(
        hdc,
        0, 0, g_bitmapWidth, g_bitmapHeight,
        0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top,
        g_app.m_colorBuffer.m_color,
        &g_app.m_bitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY);
}

static LRESULT CALLBACK WindowCallback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;

    switch (message)
    {
        case WM_SIZE:
        {
            RECT rect;
            GetClientRect(window, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            ResizeBitmap(width, height);
        }
        break;

        case WM_CLOSE:
        {
            g_app.m_quit = true;
        }
        break;

        case WM_DESTROY:
        {
            g_app.m_quit = true;
        }
        break;

        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);
            DrawBitmap(deviceContext, paint.rcPaint);
            EndPaint(window, &paint);
        }
        break;

        default:
        {
            result = DefWindowProc(window, message, wparam, lparam);
        }
        break;
    }

    return result;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //
    // Create window
    //

    const wchar_t kClassName[] = L"PuzzleWindowClass";
    const wchar_t kWindowName[] = L"Manuel Freire's Puzzle Solver (2015)";

    WNDCLASS windowClass = {};
    windowClass.style = CS_OWNDC;
    windowClass.lpfnWndProc = WindowCallback;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = kClassName;

    if (!RegisterClassW(&windowClass))
    {
        Log::Error("Cannot register window class");
        return 1;
    }

    HWND window = CreateWindowExW(
        0,
        kClassName,
        kWindowName,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        512 + 16,
        512 + 38,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!window)
    {
        Log::Error("Cannot create window");
        return 1;
    }

    //
    // Load bmp
    //

    int w, h, n;
    char* img = (char*)stbi_load("lena.png", &w, &h, &n, 0);
    ImageUtils::StbToWin32Bitmap(img, w, h);
    PuzzleInput puzzleInput = PuzzleUtils::CreatePuzzle(img, w, h, 16);
    PuzzleState puzzleState = PuzzleUtils::SolvePuzzleStart(&puzzleInput);

    //
    // Core loop
    //

    while (!g_app.m_quit)
    {
        //
        // Parse Windows messages
        //

        MSG message;

        while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                g_app.m_quit = true;
            }

            TranslateMessage(&message);
            DispatchMessageA(&message);
        }

        //
        // Render
        //

        PuzzleUtils::RenderPuzzle(&g_app.m_colorBuffer, puzzleState);

        //
        // Draw colour buffer
        //

        HDC hdc = GetDC(window);
        RECT rect;
        GetClientRect(window, &rect);
        DrawBitmap(hdc, rect);
        ReleaseDC(window, hdc);
    }

    return 0;
}
