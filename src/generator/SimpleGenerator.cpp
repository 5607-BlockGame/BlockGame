//
// Created by Andrew Gazelka on 4/21/21.
//

#include "SimpleGenerator.h"

Chunk SimpleGenerator::Generate(ChunkCoord coord) {

    Chunk chunk;

    for (size_t x = 0; x < CHUNK_WIDTH; ++x) {
        for (size_t y = 0; y < CHUNK_WIDTH; ++y) {
            for (size_t z = 0; z < CHUNK_HEIGHT; ++z) {

                Vec3D<size_t> location(x, y, z);

                Block &set = chunk[location];

                if (z == 0) {
                    set = Block(BlockType::BEDROCK); // bottom is bedrock
                } else if (z <= 200) {
                    set = Block(BlockType::STONE); // up to 200 is stone
                } else {
                    set = Block(BlockType::AIR); // rest is air
                }
            }
        }
    }

    return chunk;
}
