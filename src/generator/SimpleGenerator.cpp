//
// Created by Andrew Gazelka on 4/21/21.
//

#include <SimplexNoise/src/SimplexNoise.h>
#include "SimpleGenerator.h"

Chunk SimpleGenerator::Generate(ChunkCoord coord) {

    Chunk chunk;

    int startX = coord.x << 4;
    int startY = coord.y << 4;

    for (size_t x = 0; x < CHUNK_WIDTH; ++x) {
        for (size_t y = 0; y < CHUNK_WIDTH; ++y) {

            float actualX = startX + x;
            float actualY = startY + y;

            float sx = actualX * 0.1f;
            float sy = actualY * 0.1f;


            // [0, 1]
            float noiseVal = (SimplexNoise::noise(sx, sy) + 1.0f) / 2.0f;

            int height = (int)(CHUNK_HEIGHT * noiseVal);

            for (size_t z = 0; z < height; ++z) {

                Vec3D<size_t> location(x, y, z);

                Block &set = chunk[location];

                if (z == 0) {
                    set = Block(BlockType::BEDROCK); // bottom is bedrock
                } else if (z <= 24) {
                    set = Block(BlockType::STONE); // up to 200 is stone
                } else {
                    set = Block(BlockType::AIR); // rest is air
                }
            }
        }
    }

    return chunk;
}
