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
#pragma ide diagnostic ignored "cert-msc50-cpp"

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

static const int AMOUNT = 1000;

static int gWidth = 0, gHeight = 0;
static Camera gCamera(glm::vec3(0.0f, 0.0f, 7.5f));
static glm::mat4 gModelMatrices[AMOUNT];
static CompoundShader* gShader = nullptr, * gInstanceShader = nullptr;
static Model* gRockModel = nullptr, * gPlanetModel = nullptr;
static unsigned gInstanceVbo = 0;

static void init() {
    srand(SDL_GetTicks());

    float radius = 50.0;
    float offset = 2.5f;

    for (int i = 0; i < AMOUNT; i++) {
        auto model = glm::mat4(1.0f);

        float angle = (float)i / (float)AMOUNT * 360.0f;
        float displacement = static_cast<float>(rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = static_cast<float>(rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f;
        displacement = static_cast<float>(rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        float scale = static_cast<float>(rand() % 20) / 100.0f + 0.05f;
        model = glm::scale(model, glm::vec3(scale));

        auto rotAngle = static_cast<float>(rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        gModelMatrices[i] = model;
    }

    gShader = new CompoundShader("shaders/vertex.glsl", "shaders/fragment.glsl");
    gInstanceShader = new CompoundShader("shaders/instanceVertex.glsl", "shaders/fragment.glsl");
    gRockModel = new Model("/home/admin/Downloads/rock/rock.obj");
    gPlanetModel = new Model("/home/admin/Downloads/planet/planet.obj");

    glGenBuffers(1, &gInstanceVbo);
    glBindBuffer(GL_ARRAY_BUFFER, gInstanceVbo);
    glBufferData(GL_ARRAY_BUFFER, AMOUNT * sizeof(glm::mat4), gModelMatrices, GL_STATIC_DRAW);

    for (int i = 0; i < static_cast<int>(gRockModel->meshes().size()); i++) {
        glBindVertexArray(gRockModel->meshes()[i]->vao());

        const auto vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void*>(0 * vec4Size));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void*>(1 * vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void*>(2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void*>(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }
}

static void render() {
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(gWidth) / static_cast<float>(gHeight), 0.1f, 1000.0f);
    glm::mat4 view = gCamera.viewMatrix();

    gShader->use();
    gShader->setValue("projection", projection);
    gShader->setValue("view", view);

    gInstanceShader->use();
    gInstanceShader->setValue("projection", projection);
    gInstanceShader->setValue("view", view);

    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
    model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
    gShader->use();
    gShader->setValue("model", model);
    gPlanetModel->draw(*gShader);

    for (int i = 0; i < static_cast<int>(gRockModel->meshes().size()); i++) {
        gInstanceShader->use();
        glBindVertexArray(gRockModel->meshes()[i]->vao());
        glDrawElementsInstanced(GL_TRIANGLES, gRockModel->meshes()[i]->indices().size(), GL_UNSIGNED_INT, reinterpret_cast<void*>(0), AMOUNT);
    }
}

static void clean() {
    delete gShader;
    delete gInstanceShader;
    delete gRockModel;
    delete gPlanetModel;

    glDeleteBuffers(1, &gInstanceVbo);
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

    SDL_Window* window = SDL_CreateWindow(
        "Jealno",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        (gWidth = 1000),
        (gHeight = 1000),
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    assert(window != nullptr);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    assert(glewInit() == GLEW_OK);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
//    glEnable(GL_CULL_FACE);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
