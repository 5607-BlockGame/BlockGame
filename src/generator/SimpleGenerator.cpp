//
// Created by Andrew Gazelka on 4/21/21.
//

#include <SimplexNoise/src/SimplexNoise.h>
#include "SimpleGenerator.h"

Chunk SimpleGenerator::Generate(ChunkCoord coord) {

    Chunk chunk;

    long startX = coord.x << 4;
    long startY = coord.y << 4;

    for (size_t x = 0; x < CHUNK_WIDTH; ++x) {
        for (size_t y = 0; y < CHUNK_WIDTH; ++y) {

            auto actualX = (float) (startX + (long) x);
            auto actualY = (float) (startY + (long) y);

            float sx = actualX * 0.01f;
            float sy = actualY * 0.01f;

            // [0, 1]
            float noiseVal = (SimplexNoise::noise(sx, sy) + 1.0f) / 2.0f;

            int height = (int)(30 * noiseVal) + 1;

            for (size_t z = 0; z < height; ++z) {

                Vec3D<size_t> location(x, y, z);

                Block &set = chunk[location];

                if (z == 0) {
                    set = Block(BlockType::BEDROCK); // bottom is bedrock
                } else {
                    set = Block(BlockType::STONE); // up to 200 is stone
                }
            }
        }
    }

    return chunk;
}
