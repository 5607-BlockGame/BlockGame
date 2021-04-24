#pragma once


#include "Generator.h"

class SimpleGenerator: public Generator {
    Chunk Generate(ChunkCoord coord) override;
};

