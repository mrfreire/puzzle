#pragma once

static const int kMaxSideCount = 64;
static const int kMaxPiecesCount = kMaxSideCount * kMaxSideCount;
static const int kSizeOfPixel = 4;

enum class PieceEdge
{
    North,
    East,
    South,
    West,

    Count
};

struct PuzzleInput
{
    // NOTE: Edge data is stored in a 4 x kMaxPuzzlePieces layout to improve cache performance when
    // traversing, because we'll typically look for "the next piece with a given West edge ID of x".

    char* m_pieces;  // dynamically allocated
    const char* m_img;
    int m_pieceEdgesIds[(int)PieceEdge::Count][kMaxPiecesCount];  // Id is -1 if it's a puzzle edge
    int m_sideCount;
    int m_pieceW;
    int m_pieceH;

    PuzzleInput(int sideCount, int pieceW, int pieceH)
        : m_pieces(nullptr)
        , m_sideCount(sideCount)
        , m_pieceW(pieceW)
        , m_pieceH(pieceH)
    {}
};

struct PuzzleState
{
    const PuzzleInput* m_input;
    int m_currentSolution[kMaxPiecesCount];  // values are piece indices
    int m_foundCount;
    int m_stepCount;
    
    PuzzleState(const PuzzleInput* input)
        : m_input(input)
        , m_foundCount(0)
        , m_stepCount(0)
    {}
};

// Forward declarations
struct ColorBuffer;

namespace PuzzleUtils
{
    // Create a puzzle out of a source image. The puzzle will have sideCount x sideCount pieces.
    PuzzleInput CreatePuzzle(const char* img, int imgW, int imgH, int sideCount);
    
    // Initialise the puzzle state.
    PuzzleState SolvePuzzleStart(const PuzzleInput* input);
    
    // Perform the next step in the solution of the puzzle. This allows us to visualise the
    // algorithm.
    void SolvePuzzleStep(PuzzleState* state);
    
    // Call after SolvePuzzleStep to know when the algorithm is complete.
    bool IsPuzzleSolved(const PuzzleState& state);

    void RenderPuzzle(ColorBuffer* buffer, const PuzzleState& state);
}
