//
// Created by Andrew Gazelka on 4/21/21.
//

#include "Block.h"

Block::Block(BlockType type) : type(type) {}

Block::Block(BlockType type, char data) : type(type), data(data) {}
