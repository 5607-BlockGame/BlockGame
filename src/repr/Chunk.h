#pragma once

#include "Block.h"
#include <array>
#include <boost/format.hpp>
#include <sys/stat.h>
#include <fstream>
#include "location.h"
#include <optional>


const size_t CHUNK_WIDTH = 16;
const size_t CHUNK_SLICE = CHUNK_WIDTH * CHUNK_WIDTH;
const size_t CHUNK_HEIGHT = 256; // super large height that probably won't work
const size_t CHUNK_SIZE = CHUNK_HEIGHT * CHUNK_SLICE;
const Block EMPTY_BLOCK(BlockType::AIR);

struct Chunk {


    std::array<Block, CHUNK_SIZE> elements;

    Chunk(){
        elements = std::array<Block, CHUNK_SIZE>();
    }

    static ChunkCoord Location(PlayerLoc loc) {
        const long x = (long) round(loc.x);
        const long z = (long) round(loc.z);
        return {(int) x >> 4, (int) z >> 4};
    }

    static int BoundY(int y) {
        if (y < 0) return 0;
        if (y >= CHUNK_HEIGHT) return CHUNK_HEIGHT - 1;
        return y;
    }

    static std::optional<Chunk> ReadFromFile(ChunkCoord coord) {
        auto is = GetFileIn(coord);
        if (!is) return {};

        Chunk chunk;

        BlockType type;
        char data;
        std::ifstream stream;
        size_t idx = 0;
        while (is->read(reinterpret_cast<char *>(&type), sizeof(type) != 0)) {
            is->read(reinterpret_cast<char *>(&data), sizeof(data) != 0);
            chunk.elements[idx] = Block(type, data);
        }

        return chunk;
    }

    void WriteToFile(ChunkCoord coord) {
        auto os = GetFileOut(coord);

        for (const Block &item : elements) {
            const auto[type, data] = item;
            os.write(reinterpret_cast<const char *>(&type), sizeof(type));
            os.write(reinterpret_cast<const char *>(&data), sizeof(data));
        }

        os.close();
    }

    [[nodiscard]] Block GetBlock(Vec3D<size_t> location) const {
        const auto idx = CHUNK_SLICE * location.z + CHUNK_WIDTH * location.y + location.z;
        assert(idx < CHUNK_SIZE);
        return elements[idx];
    }

    inline Block &operator[](Vec3D<size_t> location) {
        return GetBlock(location);
    };

    Block &GetBlock(Vec3D<size_t> location) {
        assert(location.x >= 0);
        assert(location.y >= 0);
        assert(location.x < CHUNK_WIDTH);
        assert(location.y < CHUNK_WIDTH);
        assert(location.z >= 0);
        assert(location.z < CHUNK_HEIGHT);

        // 15 + 16(15)
        const auto idx = CHUNK_SLICE * location.z + CHUNK_WIDTH * location.y + location.x;
        assert(idx < CHUNK_SIZE);
        return elements[idx];
    }

private:
    static std::string FileName(ChunkCoord coord) {
        auto format = boost::format("%1%-%2%.chunk") % coord.x % coord.y;
        return format.str();
    }

    /**
     * From https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
     */
    static std::optional<std::ifstream> GetFileIn(ChunkCoord coord) {
        auto name = FileName(coord);
        std::ifstream f(name.c_str());
        if (!f.good())return {};
        return f;
    }

    static std::ofstream GetFileOut(ChunkCoord coord) { // TODO: create if DNE
        auto name = FileName(coord);
        std::ofstream f(name.c_str());
        return f;
    }

};

