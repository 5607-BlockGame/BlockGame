#pragma once

#include "location.h"
#include "Chunk.h"

class Generator {
public:
    virtual Chunk Generate(ChunkCoord coord) = 0;
};

