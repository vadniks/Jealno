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

#include "Mesh.hpp"
#include "CompoundShader.hpp"
#include <vector>
#include <string>
#include <assimp/scene.h>

class Model {
private:
    std::vector<Mesh*> mMeshes;
    std::string mDirectory;
    std::vector<Texture> mLoadedTextures;
public:
    explicit Model(const std::string& path);
    Model(const Model&) = delete;
    Model(Model&&) = delete;

    ~Model();

    Model& operator =(const Model&) = delete;
    Model& operator =(Model&&) = delete;

    void draw(CompoundShader* shader, const glm::vec4& color);
private:
    void processNode(aiNode* node, const aiScene* scene);
    Mesh* processMesh(aiMesh* mesh);
};
