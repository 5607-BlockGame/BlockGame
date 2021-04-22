#pragma once

#include "Block.h"
#include <array>
#include "location.h"


const size_t CHUNK_WIDTH = 16;
const size_t CHUNK_SLICE = CHUNK_WIDTH * CHUNK_WIDTH;
const size_t CHUNK_HEIGHT = 65536; // super large height that probably won't work
const size_t CHUNK_SIZE = CHUNK_HEIGHT * CHUNK_SLICE;

struct Chunk {

    Chunk();

    std::array<Block, CHUNK_SIZE> elements;

    [[nodiscard]] Block GetBlock(Vec3D<size_t> location) const {
        const auto idx = CHUNK_SLICE * location.z + CHUNK_WIDTH * location.y + location.z;
        assert(idx < CHUNK_SIZE);
        return elements[idx];
    }

    inline Block &operator[](Vec3D<size_t> location) {
        return GetBlock(location);
    };

    Block &GetBlock(Vec3D<size_t> location) {
        const auto idx = CHUNK_SLICE * location.z + CHUNK_WIDTH * location.y + location.z;
        assert(idx < CHUNK_SIZE);
        return elements[idx];
    }

};

