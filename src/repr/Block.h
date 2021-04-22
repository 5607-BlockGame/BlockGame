#pragma once

#include <cstdlib>

enum class BlockType: std::size_t {
    AIR = 0,
    STONE,
    WOOD,
    BEDROCK
};

struct Block {
    BlockType type;
    char data = 0;

    Block(BlockType type);
};

