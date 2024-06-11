/*
 * Jealno - an OpenGL 3D game.
 * Copyright (C) 2024 Vadim Nikolaev (https://github.com/vadniks).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma ide diagnostic ignored "NullDereference"

#include "Camera.hpp"
#include "CompoundShader.hpp"
#include "Model.hpp"
#include <cassert>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

enum Chip {
    NONE = 0,
    WHITE = 1,
    BLACK = 2
};

static const int SHADOW_SIZE = 4096, FIELD_SIZE = 8;

static int gWidth = 0, gHeight = 0;
static Camera gCamera(glm::vec3(0.9f, 2.1f, 2.9f), glm::vec3(0.0f, 1.0f, 0.0f), -89.7f, -47.3f);
static CompoundShader* gObjectShader = nullptr, * gDepthShader = nullptr, * gLightShader = nullptr, * gOutlineShader = nullptr;
static Model* gTileModel = nullptr, * gChipModel = nullptr, * gCubeModel = nullptr;
static unsigned gDepthMapFbo, gDepthMap;
static glm::vec3 gLightPos(-2.0f, 4.0f, -1.0f);

static Chip gChips[FIELD_SIZE][FIELD_SIZE];

static void init() {
    gObjectShader = new CompoundShader("shaders/objectVertex.glsl", "shaders/objectFragment.glsl");
    gObjectShader->use();
    gObjectShader->setValue("shadowMap", 0);

    gDepthShader = new CompoundShader("shaders/depthVertex.glsl", "shaders/depthFragment.glsl");
    gLightShader = new CompoundShader("shaders/lightVertex.glsl", "shaders/lightFragment.glsl");
    gOutlineShader = new CompoundShader("shaders/outlineVertex.glsl", "shaders/outlineFragment.glsl");

    gTileModel = new Model("models/tile/tile.obj");
    gChipModel = new Model("models/chip/chip.obj");
    gCubeModel = new Model("models/cube/cube.obj");

    glGenTextures(1, &gDepthMap);
    glBindTexture(GL_TEXTURE_2D, gDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_SIZE, SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (float[4]) {1.0f, 1.0f, 1.0f, 1.0f});

    glGenFramebuffers(1, &gDepthMapFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gDepthMapFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    for (int i = 0; i < FIELD_SIZE; i++) {
        for (int j = 0; j < FIELD_SIZE; j++)
            gChips[j][i] = Chip::NONE;
    }

    for (int i = 0, k = 0; i < FIELD_SIZE; i++) {
        for (int j = 0; j < 3; j++) {
            if (k % 2 == 0)
                gChips[j][i] = Chip::WHITE;
            k++;
        }
    }

    for (int i = 0, k = 0; i < FIELD_SIZE; i++) {
        for (int j = 5; j < FIELD_SIZE; j++) {
            if ((k + 1) % 2 == 0)
                gChips[j][i] = Chip::BLACK;
            k++;
        }
    }
}

static void renderScene(CompoundShader* shader, bool first) {
    glStencilMask(0x00);

    for (int i = 0; i < FIELD_SIZE; i++) {
        for (int j = 0; j < FIELD_SIZE; j++) {
            auto tileModel = glm::mat4(1.0f);
            tileModel = glm::translate(tileModel, glm::vec3(static_cast<float>(i) * 2.5f / 10.0f, 0.0f, static_cast<float>(j) * 2.5f / 10.0f));
            tileModel = glm::scale(tileModel, glm::vec3(0.125f));

            shader->use();
            shader->setValue("model", tileModel);
            shader->setValue("objectColor", (i + j) % 2 == 0 ? glm::vec3(0.125f) : glm::vec3(1.0f));
            gTileModel->draw(shader, glm::vec4(0.5f));
        }
    }

    if (!first) {
        glStencilMask(0xff);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
    }

    for (int i = 0; i < FIELD_SIZE; i++) {
        for (int j = 0; j < FIELD_SIZE; j++) {
            const Chip chip = gChips[j][i];

            auto chipModel = glm::mat4(1.0f);
            chipModel = glm::translate(chipModel, glm::vec3(0.0f, 0.06f, -0.01f));
            chipModel = glm::translate(chipModel, glm::vec3(static_cast<float>(i) * 2.5f / 10.0f, 0.0f, static_cast<float>(j) * 2.5f / 10.0f));
            chipModel = glm::scale(chipModel, glm::vec3(0.45f));

            shader->use();
            shader->setValue("model", chipModel);
            shader->setValue("objectColor", chip == Chip::WHITE ? glm::vec3(1.0f) : glm::vec3(0.125f));

            if (chip != Chip::NONE)
                gChipModel->draw(shader, glm::vec4(0.5f));
        }
    }

    if (!first) {
        glStencilFunc(GL_NOTEQUAL, 1, 0xff);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
    }

    if (!first) {
        for (int i = 0; i < FIELD_SIZE; i++) {
            for (int j = 0; j < FIELD_SIZE; j++) {
                const Chip chip = gChips[j][i];

                auto chipModel = glm::mat4(1.0f);
                chipModel = glm::translate(chipModel, glm::vec3(0.0f, 0.06f, -0.01f));
                chipModel = glm::translate(chipModel, glm::vec3(static_cast<float>(i) * 2.5f / 10.0f, 0.0f, static_cast<float>(j) * 2.5f / 10.0f));
                chipModel = glm::scale(chipModel, glm::vec3(0.5f));

                gOutlineShader->use();
                gOutlineShader->setValue("model", chipModel);

                if (chip != Chip::NONE)
                    gChipModel->draw(gOutlineShader, glm::vec4(0.5f));
            }
        }
    }

    if (!first) {
        glStencilMask(0xff);
        glStencilFunc(GL_ALWAYS, 0, 0xff);
        glEnable(GL_DEPTH_TEST);
    }
}

static void render() {
    const glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
    const glm::mat4 lightView = glm::lookAt(gLightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    const glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    gDepthShader->use();
    gDepthShader->setValue("lightSpaceMatrix", lightSpaceMatrix);
    gDepthShader->setValue("model", glm::mat4(1.0f));

    glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
    glBindFramebuffer(GL_FRAMEBUFFER, gDepthMapFbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    renderScene(gDepthShader, true);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, gWidth, gHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    const glm::mat4 projection = glm::perspective(glm::radians(gCamera.zoom()), static_cast<float>(gWidth) / static_cast<float>(gHeight), 0.1f, 100.0f);
    const glm::mat4 view = gCamera.viewMatrix();

    gObjectShader->use();
    gObjectShader->setValue("projection", projection);
    gObjectShader->setValue("view", view);
    gObjectShader->setValue("viewPos", gCamera.position());
    gObjectShader->setValue("lightPos", gLightPos);
    gObjectShader->setValue("lightSpaceMatrix", lightSpaceMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gDepthMap);

    gOutlineShader->use();
    gOutlineShader->setValue("projection", projection);
    gOutlineShader->setValue("view", view);

    renderScene(gObjectShader, false);

    auto lightModelMatrix = glm::mat4(1.0f);
    lightModelMatrix = glm::translate(lightModelMatrix, gLightPos);
    lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(0.25f));

    gLightShader->use();
    gLightShader->setValue("projection", projection);
    gLightShader->setValue("view", view);
    gLightShader->setValue("model", lightModelMatrix);

    gCubeModel->draw(gLightShader, glm::vec4(1.0f));

    SDL_Delay(1000 / 60);
//    const auto pos = gCamera.position();
//    SDL_Log("%f %f | %f %f %f", gCamera.yaw(), gCamera.pitch(), pos[0], pos[1], pos[2]);
}

static void clean() {
    delete gObjectShader;
    delete gDepthShader;
    delete gLightShader;
    delete gOutlineShader;

    delete gTileModel;
    delete gChipModel;
    delete gCubeModel;

    glDeleteTextures(1, &gDepthMap);

    glDeleteFramebuffers(1, &gDepthMapFbo);
}

static void renderLoop(SDL_Window* window) {
    int width, height;
    SDL_Event event;
    bool mousePressed = false;

    init();

    while (true) {
        SDL_GL_GetDrawableSize(window, &width, &height);
        glViewport(0, 0, width, height);

        while (SDL_PollEvent(&event) == 1) {
            switch (event.type) {
                case SDL_QUIT:
                    goto end;
                case SDL_KEYDOWN:
                    Camera::Direction direction;
                    switch (event.key.keysym.sym) {
                        case SDLK_w:
                            direction = Camera::Direction::FORWARD;
                            break;
                        case SDLK_a:
                            direction = Camera::Direction::LEFT;
                            break;
                        case SDLK_s:
                            direction = Camera::Direction::BACKWARD;
                            break;
                        case SDLK_d:
                            direction = Camera::Direction::RIGHT;
                            break;
                        case SDLK_SPACE:
                            direction = Camera::Direction::UP;
                            break;
                        case SDLK_c:
                            direction = Camera::Direction::DOWN;
                            break;
                    }
                    gCamera.processKeyboard(direction);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mousePressed = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    mousePressed = false;
                    break;
                case SDL_MOUSEMOTION:
                    if (mousePressed)
                        gCamera.processMouseMovement(static_cast<float>(event.motion.xrel), static_cast<float>(event.motion.yrel));
                    break;
                case SDL_MOUSEWHEEL:
                    gCamera.processMouseScroll(static_cast<float>(event.wheel.y));
                    break;
            }
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        render();

        SDL_GL_SwapWindow(window);
    }
    end:

    clean();
}

int main() {
    assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) == 0);
    assert(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) > 0);
    assert(TTF_Init() == 0);

    SDL_version version;
    SDL_GetVersion(&version);
    assert(version.major == 2);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_Window* window = SDL_CreateWindow(
        "Jealno",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        (gWidth = 16 * 100),
        (gHeight = 9 * 100),
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    assert(window != nullptr);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    assert(glewInit() == GLEW_OK);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    SDL_GL_SetSwapInterval(1);

    renderLoop(window);

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    assert(SDL_GetNumAllocations() == 0);
    return 0;
}
