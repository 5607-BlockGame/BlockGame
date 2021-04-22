#pragma once


#include "Generator.h"

class SimpleGenerator: Generator {
    Chunk Generate(ChunkCoord coord) override;
};

