//
// Created by Andrew Gazelka on 4/13/21.
//


#include "World.h"

Element Element::Door(size_t id) {

    struct Door d{
            .id = id
    };

    return Element{
            .tag = Tag::DOOR,
            .value = {
                    .door = d
            }
    };
}

Element Element::Wall() {
    return {
            .tag = Tag::WALL
    };
}

Element Element::Empty() {
    return {
            .tag = Tag::EMPTY
    };
}

Element Element::Key(size_t id) {
    return {
            .tag = Tag::KEY,
            .value = {
                    .key = {
                            .id = id
                    }
            }
    };
}

Element Element::Start() {
    return {.tag = Tag::START};
}

Element Element::Finish() {
    return {.tag = Tag::FINISH};
}

bool Element::operator==(const Element &rhs) const {
    if (tag != rhs.tag) return false;
    switch (tag) {
        case Tag::START:
        case Tag::FINISH:
        case Tag::WALL:
        case Tag::EMPTY:
            return true;
        case Tag::DOOR:
            return value.door == rhs.value.door;
        case Tag::KEY:
            return value.key == rhs.value.key;
    }
}

bool Element::operator!=(const Element &rhs) const {
    return !(rhs == *this);
}

bool Door::operator==(const Door &rhs) const {
    return id == rhs.id;
}

bool Door::operator!=(const Door &rhs) const {
    return !(rhs == *this);
}

bool Key::operator==(const Key &rhs) const {
    return id == rhs.id;
}

bool Key::operator!=(const Key &rhs) const {
    return !(rhs == *this);
}
