//
// Created by Andrew Gazelka on 4/13/21.
//

#include <optional>
#include "MapParser.h"
#include <boost/format.hpp>

/**
 *
 * @param c The character to parse
 * @return an element if valid else empty
 */
std::optional<Element> findElement(char c) {
    switch (c) {
        case '0':
            return Element::Empty();
        case 'S':
            return Element::Start();
        case 'G':
            return Element::Finish();
        case 'W':
            return Element::Wall();
        default:
            break;
    }

    if ('a' <= c && c <= 'e') return Element::Key(c - 'a');
    if ('A' <= c && c <= 'E') return Element::Door(c - 'A');

    return {};
}


namespace MapParser {
    Map parseMap(const std::string &name) {
        std::ifstream file;
        file.open(name);

        if (!file.is_open()) {
            const auto msg = boost::format{"File %1% is not open for reading"} % name;
            throw std::invalid_argument(msg.str());
        }
        int width, height;
        file >> width >> height;

        Map map = {
                .width = static_cast<size_t>(width),
                .height = static_cast<size_t>(height),
        };

        std::string line;

        getline(file, line);

        size_t heightCount = 0;
        while (getline(file, line)) {
            heightCount += 1;
            size_t widthCount = 0;
            for (const auto &character : line) {
                const auto elem = findElement(character);
                if (!elem.has_value()) {
                    const auto msg = boost::format{"Invalid character %1% in file read"} % character;
                    throw std::invalid_argument(msg.str());
                }
                map.elements.push_back(elem.value());
                widthCount += 1;
            }
            if (widthCount != width) {
                const auto msg =
                        boost::format{"Width of elements is %1% not the specified width %2%"} % widthCount % width;
                throw std::invalid_argument(msg.str());
            }
        }
        if (heightCount != height) {
            const auto msg =
                    boost::format{"Height of elements is %1% not the specified width %2%"} % heightCount % height;
            throw std::invalid_argument(msg.str());
        }
        file.close(); // can probably be omitted
        return map;
    }
}