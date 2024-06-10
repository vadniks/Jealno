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

#include "Model.hpp"
#include <cassert>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <SDL2/SDL_image.h>

Model::Model(const std::string& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate);
    assert(scene != nullptr && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode != nullptr);

    mDirectory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

Model::~Model() {
    for (auto mesh : mMeshes)
        delete mesh;
}

void Model::draw(CompoundShader* shader, const glm::vec4& color) {
    for (auto mesh : mMeshes)
        mesh->draw(shader, color);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    for (int i = 0; i < (int) node->mNumMeshes; i++)
        mMeshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]]));

    for (int i = 0; i < (int) node->mNumChildren; i++)
        processNode(node->mChildren[i], scene);
}

Mesh* Model::processMesh(aiMesh* mesh) {
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    for (int i = 0; i < (int) mesh->mNumVertices; i++)
        vertices.push_back(Vertex{
            glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
            glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
        });

    for (int i = 0; i < (int) mesh->mNumFaces; i++) {
        const aiFace face = mesh->mFaces[i];
        for (int j = 0; j < (int) face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    return new Mesh(std::move(vertices), std::move(indices));
}

unsigned Model::textureFromFile(const std::string& path, const std::string& directory) {
    std::string filename = directory + '/' + path;

    SDL_Surface* xSurface = IMG_Load(filename.c_str());
    assert(xSurface != nullptr);

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(xSurface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(xSurface);

    unsigned texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_FreeSurface(surface);

    return texture;
}
