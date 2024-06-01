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
static unsigned gCubeVao, gCubeVbo, gCubeTexture, gPlaneVao, gPlaneVbo, gPlaneTexture, gFbo, gFramebufferTexture, gRbo, gQuadVao, gQuadVbo;
static CompoundShader* gShader = nullptr;
static CompoundShader* gScreenShader = nullptr;

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
    float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f
    };
    float planeVertices[] = {
        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    glGenVertexArrays(1, &gCubeVao);
    glGenBuffers(1, &gCubeVbo);
    glBindVertexArray(gCubeVao);
    glBindBuffer(GL_ARRAY_BUFFER, gCubeVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glBindVertexArray(0);

    glGenVertexArrays(1, &gPlaneVao);
    glGenBuffers(1, &gPlaneVbo);
    glBindVertexArray(gPlaneVao);
    glBindBuffer(GL_ARRAY_BUFFER, gPlaneVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glBindVertexArray(0);

    gCubeTexture = loadTexture("res/marble.jpg", false);
    gPlaneTexture = loadTexture("res/metal.png", false);

    gShader = new CompoundShader("shaders/vertex.glsl", "shaders/fragment.glsl");
    gShader->use();
    gShader->setValue("texture0", 0);

    glGenFramebuffers(1, &gFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gFbo);

    glGenTextures(1, &gFramebufferTexture);
    glBindTexture(GL_TEXTURE_2D, gFramebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gWidth, gHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gFramebufferTexture, 0);

    glGenRenderbuffers(1, &gRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, gRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, gWidth, gHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gRbo);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float quadVertices[] = {
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &gQuadVao);
    glGenBuffers(1, &gQuadVbo);
    glBindVertexArray(gQuadVao);
    glBindBuffer(GL_ARRAY_BUFFER, gQuadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

    gScreenShader = new CompoundShader("shaders/quadVertex.glsl", "shaders/quadFragment.glsl");
    gScreenShader->use();
    gScreenShader->setValue("screenTexture", 0);
}

static void render() {
    glm::mat4 view = gCamera.viewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.zoom()), (float) gWidth / (float) gHeight, 0.1f, 100.0f);

    gShader->use();
    gShader->setValue("view", view);
    gShader->setValue("projection", projection);

    glCullFace(GL_FRONT);

    glBindVertexArray(gPlaneVao);
    glBindTexture(GL_TEXTURE_2D, gPlaneTexture);
    gShader->use();
    gShader->setValue("model", glm::mat4(1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glCullFace(GL_BACK);

    glBindVertexArray(gCubeVao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gCubeTexture);

    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    gShader->setValue("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    gShader->setValue("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

static void clean() {
    delete gShader;

    glDeleteVertexArrays(1, &gCubeVao);
    glDeleteBuffers(1, &gCubeVbo);
    glDeleteTextures(1, &gCubeTexture);

    glDeleteVertexArrays(1, &gPlaneVao);
    glDeleteBuffers(1, &gPlaneVbo);
    glDeleteTextures(1, &gPlaneTexture);

    glDeleteFramebuffers(1, &gFbo);
    glDeleteTextures(1, &gFramebufferTexture);
    glDeleteRenderbuffers(1, &gRbo);

    glDeleteVertexArrays(1, &gQuadVao);
    glDeleteBuffers(1, &gQuadVbo);

    delete gScreenShader;
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

        glBindFramebuffer(GL_FRAMEBUFFER, gFbo);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        gScreenShader->use();
        glBindVertexArray(gQuadVao);
        glBindTexture(GL_TEXTURE_2D, gFramebufferTexture);

//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, 6);
//        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
