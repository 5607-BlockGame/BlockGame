#pragma once

#include <repr/World.h>

#include <utility>
#include <unordered_set>
#include "utils.h"

struct TexturedModel {
    Model &model;
    unsigned int textureId;

    TexturedModel(Model &model, unsigned int textureId);
};


TexturedModel::TexturedModel(Model &model, unsigned int textureId) : model(model), textureId(textureId) {}

const int SEE_Y = 16;
const int CHUNK_VIEW_DIST = 1;

class Scene {
private:
    World &world;
    unsigned int shaderProgram;
    GLint modelParam;
    GLint textureIdParam;
    GLint colorParam;
    glm::mat4 model;
    Model& blockModel;

public:

    Scene(World &world, unsigned int shaderProgram, Model &blockModel) : world(world), shaderProgram(shaderProgram), blockModel(blockModel) {
        modelParam = glGetUniformLocation(shaderProgram, "model");
        textureIdParam = glGetUniformLocation(shaderProgram, "texID");
        colorParam = glGetUniformLocation(shaderProgram, "inColor");
        model = glm::mat4(1);

    }

    void ResetModel() {
        model = glm::mat4(1);
    }

    [[nodiscard]] glm::vec3 GetStartPosition() const {
        return to_glm(world.spawnPoint);
    }

    bool IsCollision(PlayerLoc loc) {

        long iX = (long) std::round(loc.x);
        long iY = (long) std::round(loc.y);
        long iZ = (long) std::round(loc.z);

        BlockLocation blockLocation(iX, iY, iZ);

        Block block = world.getBlockAt(blockLocation);

        return !block.IsPassthrough();

    }


    void Draw(PlayerLoc loc) {
        TexturedModel model(blockModel, 0);
        Draw(0,0,0, model);
//        ChunkCoord baseChunkCoord = Chunk::Location(loc);
//
//        int baseY = (int) loc.y;
//
//        int minY = Chunk::BoundY(baseY - SEE_Y);
//        int maxY = Chunk::BoundY(baseY + SEE_Y);
//
//        for (int chunkDX = -CHUNK_VIEW_DIST; chunkDX <= CHUNK_VIEW_DIST; ++chunkDX) {
//            for (int chunkDY = -CHUNK_VIEW_DIST; chunkDY <= CHUNK_VIEW_DIST; ++chunkDY) {
//                int chunkX = baseChunkCoord.x + chunkDX;
//                int chunkY = baseChunkCoord.y + chunkDY;
//
//                ChunkCoord chunkCoord(chunkX, chunkY);
//                Chunk *chunk = world.GetChunk(chunkCoord);
//
//                for (int y = minY; y <= maxY; ++y) {
//                    for (int x = 0; x < CHUNK_WIDTH; ++x) { // TODO: is iteration in right order?
//                        for (int z = 0; z < CHUNK_WIDTH; ++z) {
//                            BlockLocation blockLocation(x, y, z);
//                            Block &block = chunk->GetBlock(blockLocation);
//                            auto blockId = static_cast<unsigned int>(block.type);
//                            TexturedModel texturedModel(blockModel, 0); // TODO: make blockId
//
//                            double actualX = loc.x + (double) chunkDX * CHUNK_WIDTH + x;
//                            double actualZ = loc.z + (double) chunkDY * CHUNK_WIDTH + z;
//                            auto actualY = (double) y;
//                            Draw(actualX, actualY, actualZ, texturedModel);
//                        }
//                    }
//                }
//            }
//        }
    }

private:

//
//    void Draw(float x, float y, float z, const Model &model, float r, float g, float b, float scale = 1.0, float rotation = 0.0) {
//        SetColor(r, g, b);
//        SetTranslation(x, y, z);
//        SetScale(scale);
//        SetRotation(rotation);
//        SendTransformations();
//        model.draw();
//        ResetModel();
//    }

    void Draw(double x, double y, double z, const TexturedModel &texturedModel, float scale = 1.0, float rotation = 0.0) {
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

    void SetTranslation(double x, double y, double z) {
        model = glm::translate(model, glm::vec3(x, y, z));
    }

    void SetRotation(float angle) {
        model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void SetTexture(unsigned int id) const {
        glUniform1i(textureIdParam, (int) id);
    }

    void SetColor(float r, float g, float b) {
        glm::vec3 colVec(r, g, b);
        glUniform3fv(colorParam, 1, glm::value_ptr(colVec));
        SetTexture(-1);
    }

};

