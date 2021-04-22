#pragma once

#include <repr/World.h>

#include <utility>
#include <unordered_set>
#include "utils.h"

struct TexturedModel {
    Model &model;
    unsigned int textureId;
};

struct TextureData {
    TexturedModel &wallModel;
    Model &keyModel;
    TexturedModel &floorModel;
    Model &endModel;
    Model &doorModel;
};


struct SceneKey {
    int originX;
    int originY;
    float angle = 0.0;
    size_t id;
    glm::vec3 location;
};

class Scene {
private:
    TextureData textures;
    Map &map;
    unsigned int shaderProgram;
    GLint modelParam;
    GLint textureIdParam;
    GLint colorParam;
    glm::mat4 model;
    std::vector<SceneKey> keys;
    std::unordered_set<SceneKey *> grabbedKeys;

public:
    Scene(const TextureData &data, Map &map, unsigned int shaderProgram) : textures(data), map(map), shaderProgram(shaderProgram) {
        modelParam = glGetUniformLocation(shaderProgram, "model");
        textureIdParam = glGetUniformLocation(shaderProgram, "texID");
        colorParam = glGetUniformLocation(shaderProgram, "inColor");
        model = glm::mat4(1);

        for (int x = 0; x < map.width; ++x) {
            for (int y = 0; y < map.height; ++y) {
                auto element = map.GetBlock(x, y);
                if (element.tag == Tag::KEY) {
                    glm::vec3 location(x, y, -.25);
                    SceneKey key = {
                            .originX = x,
                            .originY = y,
                            .id = element.value.key.type,
                            .location = location
                    };
                    keys.push_back(key);
                }
            }
        }
    }

    void ResetModel() {
        model = glm::mat4(1);
    }

    glm::vec3 GetStartPosition() {

        for (int x = 0; x < map.width; ++x) {
            for (int y = 0; y < map.height; ++y) {
                auto element = map.GetBlock(x, y);
                if (element.tag == Tag::START) {
                    glm::vec3 res(x, y, 0.0);
                    return res;
                }
            }
        }
        throw std::logic_error("no start position");
    }


    void UpdateGrabbedKeys(glm::vec3 position, float angle) {
        for (const auto &item : grabbedKeys) {
            item->location = position;
            item->angle = angle;
        }
    }

    bool IsCollision(float x, float y) {

        if (x < -0.5 || y < -0.5) return false;

        int maxX = map.width - 1;
        int maxY = map.height - 1;

        if (x > maxX + 0.5 || y > maxY + 0.5)return false;

        int iX = (int) std::round(x);
        int iY = (int) std::round(y);

        Element element = map.GetBlock(iX, iY);

        switch (element.tag) {
            case Tag::KEY:
                HandleKey(iX, iY);
                return false;
            case Tag::START:
            case Tag::EMPTY:
                return false;
            case Tag::WALL:
                return true;
            case Tag::DOOR:
                return HandleDoor(element.value.door, iX, iY);
            case Tag::FINISH:
                HandleFinish();
                return true;
        }

    }


    void Draw() {
        for (int x = 0; x < map.width; ++x) {
            for (int y = 0; y < map.height; ++y) {
                auto element = map.GetBlock(x, y);

                auto fx = (float) x;
                auto fy = (float) y;

                switch (element.tag) {
                    case Tag::FINISH:
                        Draw(fx, fy, -.25, textures.endModel, 0.1, 0.9f, 0.0f, 0.2);
                        break;
                    case Tag::START:
                    case Tag::EMPTY:
                    case Tag::KEY: // keys are dynamic—we draw them elsewhere
                        // no special drawing
                        break;
                    case Tag::DOOR:
                        Draw(fx, fy, 0.0f, textures.doorModel, (float) element.value.door.type / 5.0f, 0.0f, 0.0f);
                        break;
                    case Tag::WALL:
                        Draw(fx, fy, 0.0f, textures.wallModel);
                        break;
                }
                Draw(fx, fy, -1.0f, textures.floorModel);
            }
        }

        for (const auto &key: keys) {
            auto id = key.id;
            Draw(key.location[0], key.location[1], key.location[2], textures.keyModel, (float) id / 5.0f, 0.0f, 0.0f, 0.2, -key.angle);
        }
    }

private:

    /**
     *
     * @param door
     * @param iX
     * @param iY
     * @return  True if collide else false
     */
    bool HandleDoor(const Door &door, int iX, int iY) {
        for (const auto &grabbedKey : grabbedKeys) {
            if (door.type == grabbedKey->id) {

                // the grabbed grabbedKey disappears
                grabbedKeys.erase(grabbedKey);

                for (int i = 0; i < keys.size(); ++i) {
                    auto &key = keys[i];
                    if (key.originX == grabbedKey->originX && key.originY == grabbedKey->originY) {
                        keys.erase(keys.begin() + i);
                        goto end;
                    }
                }

                end:

                for (const auto &key: keys) {
                }


                // the door disappears
                *map.GetElementRef(iX, iY) = Element::Empty();

                return false;
            }
        }
        return true;
    }

    void HandleFinish() {
        for (auto &item : map.elements) {
            item = Element::Empty();
        }
    }

    void HandleKey(int iX, int iY) {

        SceneKey *key = nullptr;
        for (auto &item : keys) {
            if (item.originX == iX && item.originY == iY) {
                key = &item;
            }
        }
        assert(key != nullptr);
        grabbedKeys.insert(key);
        *map.GetElementRef(iX, iY) = Element::Empty();
    }

    void Draw(float x, float y, float z, const Model &model, float r, float g, float b, float scale = 1.0, float rotation = 0.0) {
        SetColor(r, g, b);
        SetTranslation(x, y, z);
        SetScale(scale);
        SetRotation(rotation);
        SendTransformations();
        model.draw();
        ResetModel();
    }

    void Draw(float x, float y, float z, const TexturedModel &texturedModel, float scale = 1.0, float rotation = 0.0) {
        SetTexture(texturedModel.textureId);
        SetTranslation(x, y, z);
        SetScale(scale);
        SetRotation(rotation);
        SendTransformations();
        texturedModel.model.draw();
        ResetModel();
    }

    void SendTransformations() {
        glUniformMatrix4fv(modelParam, 1, GL_FALSE, glm::value_ptr(model));
    }

    void SetScale(float x) {
        model = glm::scale(model, glm::vec3(x, x, x));
    }

    void SetTranslation(float x, float y, float z = 0) {
        model = glm::translate(model, glm::vec3(x, y, z));
    }

    void SetRotation(float angle) {
        model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void SetTexture(int id) {
        glUniform1i(textureIdParam, id);
    }

    void SetColor(float r, float g, float b) {
        glm::vec3 colVec(r, g, b);
        glUniform3fv(colorParam, 1, glm::value_ptr(colVec));
        SetTexture(-1);
    }

};

