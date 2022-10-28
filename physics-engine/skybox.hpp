#pragma once
#include "texture.hpp"
#include "shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static float skyboxVertices[] = { -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f };

class Skybox {
public:
    Skybox(glm::mat4 _projectionMatrix, texture_t _skyboxTexture) : mSkyboxShader(_shaderProgram) {
        mProjectionMatrix = _projectionMatrix;
        mSkyboxTexture = _skyboxTexture;
    }
    void init() {
        mSkyboxShader.set_int("skybox", 0);
        glGenVertexArrays(1, &mVao);
        glGenBuffers(1, &mVbo);
        glBindVertexArray(mVao);
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
        glBindVertexArray(0);
    }
    void draw(glm::mat3 view) {
        mTrimmedViewMatrix = view;
        glDepthMask(GL_FALSE);

            mSkyboxShader.use();
            mSkyboxShader.set_mat4("projection", mProjectionMatrix);
            mSkyboxShader.set_mat4("view", mTrimmedViewMatrix);
            glBindVertexArray(mVao);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, mSkyboxTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthMask(GL_TRUE);
    }
private:
    GLuint mVao;
    GLuint mVbo;
    texture_t mSkyboxTexture;
    Shader& mSkyboxShader;

    glm::mat4 mProjectionMatrix;
    glm::mat3 mTrimmedViewMatrix;
};