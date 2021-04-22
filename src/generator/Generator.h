#pragma once

#include "repr/location.h"
#include "repr/Chunk.h"

class Generator {
public:
    virtual Chunk Generate(ChunkCoord coord) = 0;
};

