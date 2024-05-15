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
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

Model::Model(const std::string& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
    assert(scene != nullptr && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode != nullptr);

    mDirectory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

Model::~Model() {
    for (auto mesh : mMeshes)
        delete mesh;
}

void Model::draw(CompoundShader& shader) {
    for (auto mesh : mMeshes)
        mesh->draw(shader);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    for (int i = 0; i < (int) node->mNumMeshes; i++)
        mMeshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene));

    for(int i = 0; i < (int) node->mNumChildren; i++)
        processNode(node->mChildren[i], scene);
}

Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
    std::vector<Texture> textures;

    for(int i = 0; i < (int) mesh->mNumVertices; i++)
        vertices.push_back(Vertex{
            glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
            glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z),
            mesh->mTextureCoords[0] != nullptr
                ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
                : glm::vec2(0.0f, 0.0f)
        });

    for (int i = 0; i < (int) mesh->mNumFaces; i++) {
        const aiFace face = mesh->mFaces[i];
        for (int j = 0; j < (int) face.mNumIndices; i++)
            indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0) {
        const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        const std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, Texture::TYPE_DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        const std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, Texture::TYPE_SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return new Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

std::vector<Texture> Model::loadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName) {

}
