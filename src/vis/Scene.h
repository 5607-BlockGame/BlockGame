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

//const int SEE_Z = 40;
const int CHUNK_VIEW_DIST = 6;

class Scene {
private:
    World &world;
    unsigned int shaderProgram;
    unsigned int shader2DProgram;
    GLint modelParam;
    GLint textureIdParam;
    GLint colorParam;
    glm::mat4 model;
    Model &blockModel;
    CrossHair &crossHair;
    BlockLocation selectedBlock;

public:

    Scene(World &world, unsigned int shaderProgram, unsigned int shader2DProgram, Model &blockModel, CrossHair &crossHair) : world(world), shaderProgram(shaderProgram),
                                                                                                                             shader2DProgram(shader2DProgram), blockModel(blockModel),
                                                                                                                             crossHair(crossHair),
                                                                                                                             selectedBlock(BlockLocation(0,0,0)) {
        modelParam = glGetUniformLocation(shaderProgram, "model");
        textureIdParam = glGetUniformLocation(shaderProgram, "texID");
        colorParam = glGetUniformLocation(shaderProgram, "inColor");
        model = glm::mat4(1);

    }

    inline void ResetModel() {
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

        // out of bounds
        if (iZ < 0 || iZ >= CHUNK_HEIGHT) return false;

        Block block = world.getBlockAt(blockLocation);

        return !block.IsPassthrough();

    }

    bool GetBlockLocation(glm::vec3 loc, BlockLocation *blockLocation) {
        long iX = (long) std::round(loc.x);
        long iY = (long) std::round(loc.y);
        long iZ = (long) std::round(loc.z);

        *blockLocation = BlockLocation(iX, iY, iZ);
        if (iZ < 0 || iZ >= CHUNK_HEIGHT) return false;

        Block block = world.getBlockAt(*blockLocation);
        return !block.IsPassthrough();
    }

    void selectBlock(BlockLocation location) {
        selectedBlock = location;
    }


    void Draw(PlayerLoc playerLoc) {
        ChunkCoord baseChunkCoord = Chunk::Location(playerLoc);

        int baseZ = (int) playerLoc.z;

//        int minZ = Chunk::BoundZ(baseZ - SEE_Z);
//        int maxZ = Chunk::BoundZ(baseZ + SEE_Z);

//        TexturedModel texturedModel(blockModel, 0); // TODO: make blockId

        for (int chunkDX = -CHUNK_VIEW_DIST; chunkDX <= CHUNK_VIEW_DIST; ++chunkDX) {
            for (int chunkDY = -CHUNK_VIEW_DIST; chunkDY <= CHUNK_VIEW_DIST; ++chunkDY) {
                int chunkX = baseChunkCoord.x + chunkDX;
                int chunkY = baseChunkCoord.y + chunkDY;

                auto chunkStartX = (double) (chunkX << 4);
                auto chunkStartY = (double) (chunkY << 4);

                ChunkCoord chunkCoord(chunkX, chunkY);
                Chunk *chunk = world.GetChunk(chunkCoord);

                for (int blockX = 0; blockX < CHUNK_WIDTH; ++blockX) { // TODO: is iteration in right order?
                    for (int blockY = 0; blockY < CHUNK_WIDTH; ++blockY) {
                        const TopBlock &topBlock = chunk->GetTopBlock(blockX, blockY);

                        double actualX = chunkStartX + blockX;
                        double actualY = chunkStartY + blockY;
                        auto actualZ = (double) topBlock.z;

                        BlockLocation loc(actualX,actualY,actualZ);
                        float r = actualZ / (CHUNK_HEIGHT/5);  // originally 0.7
                        if (selectedBlock == loc) {  // draw wireframe around selected block
                            Draw((float) actualX, (float) actualY, (float) actualZ, blockModel, r, 0.2, 0.3);
                            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                            Draw((float) actualX, (float) actualY, (float) actualZ, blockModel, 0, 1, 0, 1.03);
                            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                        } else {
                            Draw((float) actualX, (float) actualY, (float) actualZ, blockModel, r, 0.2, 0.3);
                        }
                    }
                }
            }
        }
    }

    // https://stackoverflow.com/questions/56229367/opengl-make-object-stick-to-camera
    void DrawPlayer(PlayerLoc loc, glm::vec3 forward, float rotation, glm::mat4 view) {
        glm::vec3 up(0.0, 0.0, 1.0);
        glm::vec3 right = glm::normalize(glm::cross(up, forward));
        double handX = 0.05f;
        double handY = -0.01f;
        double handZ = -1.0f;
        DrawHand(handX, handY, handZ, rotation, view);
    }

    void DrawCrossHair(double x, double y) {
        double scaleX = 40;
        double scaleY = 40;
        DrawCrossHair(x, y, scaleX, scaleY);
    }

private:


    void Draw(float x, float y, float z, const Model &model, float r, float g, float b, float scale = 1.0) {
        SetColor(r, g, b);
        SetTranslation(x, y, z);
        SetScale(scale);
        SendTransformations();
        model.draw();
        ResetModel();
    }

    void DrawHand(double x, double y, double z, float rotation, glm::mat4 view) {
        SetColor(1.0, 0.86, 0.67);        // Skin color
        SetTranslation(x, y, z);
        SetRotationX(rotation);
        SetScale(0.02, 0.02, 0.2);
        model = glm::inverse(view) * model;
        SendTransformations();
        blockModel.draw();
        ResetModel();
    }

    void Draw(double x, double y, double z, const TexturedModel &texturedModel, float scale = 1.0, float rotation = 0.0) {
        SetTexture(texturedModel.textureId);
        SetTranslation(x, y, z);
        SetScale(scale);
        SetRotationZ(rotation);         // Not sure which rotation axis we want here
        SendTransformations();
        texturedModel.model.draw();
        ResetModel();
    }

    void DrawCrossHair(double x, double y, float scaleX, float scaleY) {
        SetTranslation(x, y, 0.0f);
        SetScale(scaleX, scaleY, 1.0f);
        GLint model2D = glGetUniformLocation(shader2DProgram, "model");
        glUniformMatrix4fv(model2D, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(shader2DProgram, "image"), crossHair.textureId);
        crossHair.draw();
    }

    inline void SendTransformations() {
        glUniformMatrix4fv(modelParam, 1, GL_FALSE, glm::value_ptr(model));
    }

    void SetScale(float x) {
        model = glm::scale(model, glm::vec3(x, x, x));
    }

    void SetScale(float x, float y, float z) {
        model = glm::scale(model, glm::vec3(x, y, z));
    }

    inline void SetTranslation(double x, double y, double z) {
        model = glm::translate(model, glm::vec3(x, y, z));
    }

    void SetRotationX(float angle) {
        model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
    }

    void SetRotationY(float angle) {
        model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void SetRotationZ(float angle) {
        model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void SetTexture(unsigned int id) const {
        glUniform1i(textureIdParam, (int) id);
    }

    inline void SetColor(float r, float g, float b) {
        glm::vec3 colVec(r, g, b);
        glUniform3fv(colorParam, 1, glm::value_ptr(colVec));
        SetTexture(-1);
    }

};

