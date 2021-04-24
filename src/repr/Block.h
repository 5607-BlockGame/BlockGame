#pragma once

#include <cstdlib>

enum class BlockType : std::size_t {
    AIR = 0,
    STONE,
    WOOD,
    BEDROCK
};

inline bool isPassthrough(BlockType blockType) {
    switch (blockType) {
        case BlockType::AIR:
            return true;
        case BlockType::STONE:
        case BlockType::WOOD:
        case BlockType::BEDROCK:
            return false;
    }
}


struct Block {
    BlockType type;
    char data = 0;

    Block() : type(BlockType::AIR) {}

    Block(BlockType type);

    Block(BlockType type, char data);

    [[nodiscard]] bool IsPassthrough() const {
        return isPassthrough(type);
    }
};


