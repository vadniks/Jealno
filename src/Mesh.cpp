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

#include "Mesh.hpp"
#include <memory>

Mesh::Mesh(
    std::vector<Vertex>&& vertices,
    std::vector<unsigned>&& indices,
    std::vector<Texture>&& textures
) :
    mVao(0),
    mVbo(0),
    mEbo(0),
    mVertices(std::move(vertices)),
    mIndices(std::move(indices)),
    mTextures(std::move(textures))
{
    glGenVertexArrays(1, &mVao);
    glGenBuffers(1, &mVbo);
    glGenBuffers(1, &mEbo);

    glBindVertexArray(mVao);

    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, (long) (mVertices.size() * sizeof(Vertex)), &(mVertices[0]), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (long) (mIndices.size() * sizeof(unsigned)), &(mIndices[0]), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));

    glBindVertexArray(0);
}

Mesh::~Mesh() {
    for (const auto& i : mTextures)
        glDeleteTextures(1, &(i.id));

    glDeleteVertexArrays(1, &mVao);
    glDeleteBuffers(1, &mVbo);
    glDeleteBuffers(1, &mEbo);
}

void Mesh::draw(CompoundShader& shader) {
    shader.use();

    for (int i = 0/*, diffuseNr = 0, specularNr = 0*/; i < (int) mTextures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);

//        std::string number;
        std::string name = mTextures[i].type;
//        if (name == Texture::TYPE_DIFFUSE)
//            number = std::to_string(diffuseNr++);
//        else if (name == Texture::TYPE_SPECULAR)
//            number = std::to_string(specularNr++);

//        shader.setValue(std::string("material.").append(name).append(number), i);
        if (name == Texture::TYPE_DIFFUSE)
            shader.setValue("textureDiffuse", i);

        glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
    }

    glActiveTexture(0);

    glBindVertexArray(mVao);
    glDrawElements(GL_TRIANGLES, (int) mIndices.size(), GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
    glBindVertexArray(0);
}

unsigned Mesh::vao() {
    return mVao;
}

const std::vector<unsigned>& Mesh::indices() {
    return mIndices;
}
