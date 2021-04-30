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


struct TopBlock {
    size_t z;
    Block block;
    TopBlock(): z(0){}
};

struct Chunk {
    Chunk(){
        elements = std::array<Block, CHUNK_SIZE>();
        topBlocks = std::array<TopBlock, CHUNK_SLICE>();
    }

private:
    std::array<Block, CHUNK_SIZE> elements;
    std::array<TopBlock, CHUNK_SLICE> topBlocks;
public:


    static ChunkCoord Location(PlayerLoc loc) {
        const long x = (long) round(loc.x);
        const long y = (long) round(loc.y);
        return {(int) x >> 4, (int) y >> 4};
    }

    static int BoundZ(int z) {
        if (z < 0) return 0;
        if (z >= CHUNK_HEIGHT) return CHUNK_HEIGHT - 1;
        return z;
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

    inline const Block &operator[](Vec3D<size_t> location) {
        return GetBlock(location);
    };

    void SetBlock(Vec3D<size_t> location, Block block){
        auto [x,y,z] = location;
        assert(x >= 0);
        assert(x < CHUNK_WIDTH);
        assert(y >= 0);
        assert(y < CHUNK_WIDTH);
        assert(z >= 0);
        assert(z < CHUNK_HEIGHT);

        auto& topBlock = GetTopBlockRef(x, y);
        if(z >= topBlock.z && block.type != BlockType::AIR){
            topBlock.z = z;
            topBlock.block = block;
        }
        else if(block.type == BlockType::AIR && topBlock.z == location.y){
            UpdateTopBlock(x,y, topBlock);
        }
        const auto idx = CHUNK_SLICE * location.z + CHUNK_WIDTH * location.y + location.x;
        elements[idx] = block;
    }

    [[nodiscard]] const TopBlock& GetTopBlock(size_t x, size_t y) const{
        size_t idx = CHUNK_WIDTH * y + x;
        return topBlocks[idx];
    }

    [[nodiscard]] const Block &GetBlock(Vec3D<size_t> location) const {

        // 15 + 16(15)
        const auto idx = CHUNK_SLICE * location.z + CHUNK_WIDTH * location.y + location.x;
        assert(idx < CHUNK_SIZE);
        return elements[idx];
    }

    [[nodiscard]] TopBlock GetTopBlock(size_t x, size_t y){
        size_t idx = CHUNK_WIDTH * y + x;
        return topBlocks[idx];
    }


private:
    [[nodiscard]] TopBlock& GetTopBlockRef(size_t x, size_t y){
        size_t idx = CHUNK_WIDTH * y + x;
        return topBlocks[idx];
    }

    void UpdateTopBlock(size_t x, size_t y, TopBlock &previous){
        for (int z = previous.z - 1; z >= 0 ; --z) {
            Vec3D<size_t> location(x,y,z);
            auto &block = GetBlock(location);
            if(block.type != BlockType::AIR){
               previous.z = z;
               previous.block = block;
               return;
            }
        }
    }

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

