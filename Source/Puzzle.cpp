#include "Puzzle.h"

#include "ColorBuffer.h"
#include "Utils/BufferUtils.h"
#include "Utils/Log.h"
#include "Utils/RandomUtils.h"

#include "External/pow2assert.h"

#include <string.h>

PuzzleInput PuzzleUtils::CreatePuzzle(const char* img, int imgW, int imgH, int sideCount)
{
    //
    // Validate input
    //
    
    POW2_ASSERT(img);
    POW2_ASSERT(imgW > 0);
    POW2_ASSERT(imgH > 0);
    POW2_ASSERT(sideCount > 1);

    if (sideCount > kMaxSideCount)
    {
        Log::Error("Too many pieces (%d per side). Max side count is %d", sideCount, kMaxSideCount);
        return PuzzleInput(0, 0, 0);
    }

    const int piecePixelRows = imgW / sideCount;
    const int piecePixelCols = imgH / sideCount;
    const int pieceRowBytes = piecePixelCols * kSizeOfPixel;
    const int imgRowBytes = imgW * kSizeOfPixel;

    if (piecePixelRows * sideCount < imgW)
    {
        Log::Warning("Image width is not a multiple of sideCount. Losing pixels.");
    }
    if (piecePixelCols * sideCount < imgH)
    {
        Log::Warning("Image height is not a multiple of sideCount. Losing pixels.");
    }

    //
    // Initialise pieces
    //

    PuzzleInput input(sideCount, piecePixelCols, piecePixelRows);

    input.m_pieces = new char[imgW * imgH * kSizeOfPixel];
    input.m_img = img;

    char* pieces = input.m_pieces;
    for (int i = 0; i < sideCount; ++i)
    {
        for (int j = 0; j < sideCount; ++j)
        {
            // For each piece, copy their rows contiguously into the pieces buffer

            for (int k = 0; k < piecePixelRows; ++k)
            {
                const char* src = BufferUtils::GetPointerToElement(
                    img,
                    imgW,
                    kSizeOfPixel,
                    j * input.m_pieceW,
                    i * input.m_pieceH + k);
                memcpy(pieces, src, pieceRowBytes);
                pieces += pieceRowBytes;
            }
        }
    }

    //
    // Initialise edge ids
    //

    const int vEdgesOffset = 0;
    const int hEdgesOffset = sideCount * sideCount;

    for (int o = 0; o < (int)PieceEdge::Count; ++o)
    {
        for (int i = 0; i < sideCount; ++i)
        {
            for (int j = 0; j < sideCount; ++j)
            {
                const int pieceIndex = i * sideCount + j;
                input.m_pieceEdgesIds[o][pieceIndex] = -1;
            }
        }
    }

    for (int i = 0; i < sideCount - 1; ++i)
    {
        for (int j = 0; j < sideCount; ++j)
        {
            const int pieceIndex = i * sideCount + j;
            input.m_pieceEdgesIds[(int)PieceEdge::South][pieceIndex] = vEdgesOffset + pieceIndex;
        }
    }

    for (int i = 1; i < sideCount; ++i)
    {
        for (int j = 0; j < sideCount; ++j)
        {
            const int pieceIndex = i * sideCount + j;
            const int northIndex = pieceIndex - sideCount;
            input.m_pieceEdgesIds[(int)PieceEdge::North][pieceIndex] = vEdgesOffset + northIndex;
        }
    }

    for (int i = 0; i < sideCount; ++i)
    {
        for (int j = 0; j < sideCount - 1; ++j)
        {
            const int pieceIndex = i * sideCount + j;
            input.m_pieceEdgesIds[(int)PieceEdge::East][pieceIndex] = hEdgesOffset + pieceIndex;
        }
    }

    for (int i = 0; i < sideCount; ++i)
    {
        for (int j = 1; j < sideCount; ++j)
        {
            const int pieceIndex = i * sideCount + j;
            const int westIndex = pieceIndex - 1;
            input.m_pieceEdgesIds[(int)PieceEdge::West][pieceIndex] = hEdgesOffset + westIndex;
        }
    }

    return input;
}

PuzzleState PuzzleUtils::SolvePuzzleStart(const PuzzleInput* input)
{
    PuzzleState state(input);
    
    const int piecesCount = state.m_input->m_sideCount * state.m_input->m_sideCount;
    for (int i = 0; i < piecesCount; ++i)
    {
        state.m_currentSolution[i] = i;
    }
    
    RandomUtils::Shuffle(state.m_currentSolution, piecesCount);

    state.m_foundCount = 0;
    state.m_stepCount = 0;

    return state;
}

void PuzzleUtils::SolvePuzzleStep(PuzzleState* state)
{
}

bool PuzzleUtils::IsPuzzleSolved(const PuzzleState& state)
{
    return false;
}

void PuzzleUtils::RenderPuzzle(ColorBuffer* buffer, const PuzzleState& state)
{
    //
    // Clear buffer
    //

    memset(buffer->m_color, 0x7f, buffer->m_colorBufferBytes);
    
    //
    // Render puzzle tiles
    //

    const int sideCount = state.m_input->m_sideCount;
    const int piecesCount = sideCount * sideCount;
    const int pieceRowBytes = state.m_input->m_pieceW * kSizeOfPixel;

    for (int i = 0; i < piecesCount; ++i)
    {
        int srcIndex = state.m_currentSolution[i];
        int srcRow = srcIndex / sideCount;
        int srcCol = srcIndex - (srcRow * sideCount);
        int srcMinX = srcCol * state.m_input->m_pieceW;
        int srcMinY = srcRow * state.m_input->m_pieceH;
        int dstIndex = i;
        int dstRow = dstIndex / sideCount;
        int dstCol = dstIndex - (dstRow * sideCount);
        int dstMinX = dstCol * state.m_input->m_pieceW;
        int dstMinY = dstRow * state.m_input->m_pieceH;

        const char* src = BufferUtils::GetPointerToElement(
            state.m_input->m_pieces,
            state.m_input->m_sideCount,
            pieceRowBytes,
            srcMinX,
            srcMinY);
        for (int j = 0; j < state.m_input->m_pieceH; ++j)
        {
            char* dst = BufferUtils::GetPointerToElement(
                buffer->m_color,
                buffer->m_width,
                buffer->m_bytesPerPixel,
                dstMinX,
                dstMinY + j);
            memcpy(dst, src, pieceRowBytes);
            src += pieceRowBytes;
        }
    }
}