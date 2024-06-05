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

#pragma once

#include "CompoundShader.hpp"
#include <string>
#include <vector>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
    static inline const std::string TYPE_DIFFUSE = "texture_diffuse";
    static inline const std::string TYPE_SPECULAR = "texture_specular";
};

class Mesh {
private:
    unsigned mVao, mVbo, mEbo;
public:
    std::vector<Vertex> mVertices;
    std::vector<unsigned> mIndices;
    std::vector<Texture> mTextures;
public:
    Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned>&& indices, std::vector<Texture>&& textures);
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;

    ~Mesh();

    Mesh& operator =(const Mesh&) = delete;
    Mesh& operator =(Mesh&&) = delete;

    void draw(CompoundShader& shader);

    unsigned vao();
    const std::vector<unsigned>& indices();
};
