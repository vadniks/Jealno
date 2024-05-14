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

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class CompoundShader final {
private:
    unsigned mProgramId;
public:
    CompoundShader(const std::string& vertexPath, const std::string& fragmentPath);
    CompoundShader(const CompoundShader&) = delete;
    CompoundShader(CompoundShader&&) = delete;

    ~CompoundShader();

    CompoundShader& operator =(const CompoundShader&) = delete;
    CompoundShader& operator =(CompoundShader&&) = delete;

    void use();
    void setValue(const std::string& name, bool value);
    void setValue(const std::string& name, float value);
    void setValue(const std::string& name, int value);
    void setValue(const std::string& name, const glm::vec2& value);
    void setValue(const std::string& name, const glm::vec3& value);
    void setValue(const std::string& name, const glm::vec4& value);
    void setValue(const std::string& name, const glm::mat3& value);
    void setValue(const std::string& name, const glm::mat4& value);
};
