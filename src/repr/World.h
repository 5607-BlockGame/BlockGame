#pragma once

#include <cstdlib>
#include <vector>
#include <array>
#include <unordered_map>
#include "location.h"
#include "Chunk.h"
#include "generator/Generator.h"


struct PlayerData {
    Vec3D<double> location;
};


class World {
private:
    Generator &generator;
    std::vector<PlayerData> playerData;
    std::unordered_map<ChunkCoord, Chunk> chunksInMemory;
public:
    Vec3D<double> spawnPoint = Vec3D<double>(0.0, 0.0, 255.0);

    World(Generator &generator) : generator(generator) {}

    Chunk *GetChunk(ChunkCoord coord) {
        // try memory first
        auto *chunkInMem = GetChunkInMemory(coord);
        if (chunkInMem != nullptr) return chunkInMem;

        // try disk next
//        auto chunkOnDisk = GetChunkOnDisk(coord);
//        if (chunkOnDisk) { // is optional
//            return AddChunkToMemory(chunkOnDisk.value(), coord);
//        }

        // if all else fails, generate the chunk.
        auto generatedChunk = generator.Generate(coord);
        return AddChunkToMemory(generatedChunk, coord);
    }

    Block &getBlockAt(BlockLocation location) {
        ChunkCoord chunkCoord((int) location.x >> 4, (int) location.y >> 4);
        Chunk *chunk = GetChunk(chunkCoord);

        Vec3D<long> blockLocation(location.x % 16, location.y % 16, location.z);

        if (blockLocation.x < 0) blockLocation.x = 16 + blockLocation.x;
        if (blockLocation.y < 0) blockLocation.y = 16 + blockLocation.y;

        return chunk->GetBlock(blockLocation);
    }


private:
    Chunk *GetChunkInMemory(ChunkCoord coord) {
        auto itr = chunksInMemory.find(coord);

        // DNE
        if (itr == chunksInMemory.end()) return nullptr;

        return &itr->second;
    }

    /**
     * Adds chunk to memory and returns pointer
     */
    Chunk *AddChunkToMemory(Chunk chunk, ChunkCoord coord) {
        chunksInMemory[coord] = chunk;
        return &chunksInMemory.find(coord)->second;
    }

    std::optional<Chunk> GetChunkOnDisk(ChunkCoord coord) {
        return Chunk::ReadFromFile(coord);
    }

};
