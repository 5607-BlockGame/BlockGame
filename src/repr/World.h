#pragma once

#include <cstdlib>
#include <vector>
#include <array>
#include <unordered_map>
#include "location.h"
#include "Chunk.h"
#include "Generator.h"

enum class Tag {
    START = 0,
    FINISH,
    DOOR,
    KEY,
    WALL,
    EMPTY
};


struct Door {
    size_t id;

    bool operator==(const Door &rhs) const;

    bool operator!=(const Door &rhs) const;
};

struct Key {
    size_t id;

    bool operator==(const Key &rhs) const;

    bool operator!=(const Key &rhs) const;
};

struct Empty {
    static Empty Get() {
        return {};
    }
};


struct Element {
    Tag tag;
    union Value {
        Empty empty;
        Door door;
        Key key;
    } value;

    static Element Start();

    static Element Finish();

    static Element Empty();

    static Element Wall();

    static Element Door(size_t id);

    static Element Key(size_t id);

    bool operator==(const Element &rhs) const;

    bool operator!=(const Element &rhs) const;
};

struct PlayerData {
    Vec3D<double> location;
};


class World {
public:
    Chunk *GetChunk(ChunkCoord coord) {
        // try memory first
        auto *chunkInMem = GetChunkInMemory(coord);
        if (chunkInMem != nullptr) return chunkInMem;

        // try disk next
        auto chunkOnDisk = GetChunkOnDisk(coord);
        if (chunkOnDisk) { // is optional
            return AddChunkToMemory(chunkOnDisk.value(), coord);
        }

        // if all else fails, generate the chunk.
        auto generatedChunk = generator.Generate(coord);
        return AddChunkToMemory(generatedChunk, coord);
    }


private:
    std::vector<PlayerData> playerData;
    std::unordered_map<ChunkCoord, Chunk> chunksInMemory;

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
        // TODO
    }

    Generator &generator;
};
