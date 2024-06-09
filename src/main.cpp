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

static int gWidth = 0, gHeight = 0;
static Camera gCamera(glm::vec3(0.0f, 0.0f, 7.5f));
static CompoundShader* gObjectShader = nullptr, * gLightShader = nullptr;
static Model* gTileModel = nullptr, * gChipModel = nullptr, * gCubeModel = nullptr;

static unsigned loadTexture(std::string&& path, bool clampToEdge) {
    if (!path.ends_with(".png") && !path.ends_with(".jpg"))
        assert(false);

    unsigned id;
    glGenTextures(1, &id);

    SDL_Surface* xSurface = IMG_Load(path.c_str());
    assert(xSurface != nullptr);
    SDL_Surface* surface = SDL_ConvertSurfaceFormat(xSurface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(xSurface);

    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clampToEdge ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clampToEdge ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return id;
}

static void init() {
    gObjectShader = new CompoundShader("shaders/objectVertex.glsl", "shaders/objectFragment.glsl");
    gLightShader = new CompoundShader("shaders/lightVertex.glsl", "shaders/lightFragment.glsl");

    gTileModel = new Model("models/tile/tile.obj");
    gChipModel = new Model("models/chip/chip.obj");
    gCubeModel = new Model("models/cube/cube.obj");
}

static void render() {
    const auto lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

    const glm::mat4 projection = glm::perspective(glm::radians(gCamera.zoom()), static_cast<float>(gWidth) / static_cast<float>(gHeight), 0.1f, 100.0f);
    const glm::mat4 view = gCamera.viewMatrix();

    gObjectShader->use();
    gObjectShader->setValue("projection", projection);
    gObjectShader->setValue("view", view);
    gObjectShader->setValue("lightColor", glm::vec3(1.0f));
    gObjectShader->setValue("lightPos", lightPos);
    gObjectShader->setValue("viewPos", gCamera.position());

    gLightShader->use();
    gLightShader->setValue("projection", projection);
    gLightShader->setValue("view", view);

    auto tileModel = glm::mat4(1.0f);
    tileModel = glm::translate(tileModel, glm::vec3(0.0f, 1.0f, 0.0f));
    gObjectShader->use();
    gObjectShader->setValue("model", tileModel);
    gObjectShader->setValue("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
    gTileModel->draw(gObjectShader, glm::vec4(0.5f));

    auto chipModel = glm::mat4(1.0f);
    chipModel = glm::translate(chipModel, glm::vec3(0.0f, -1.0f, 0.0f));
    gObjectShader->use();
    gObjectShader->setValue("model", chipModel);
    gObjectShader->setValue("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
    gChipModel->draw(gObjectShader, glm::vec4(0.5f));

    auto lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);
    lightModel = glm::scale(lightModel, glm::vec3(0.125f));
    gLightShader->use();
    gLightShader->setValue("model", lightModel);
    gCubeModel->draw(gLightShader, glm::vec4(1.0f));
}

static void clean() {
    delete gObjectShader;
    delete gLightShader;

    delete gTileModel;
    delete gChipModel;
    delete gCubeModel;
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
