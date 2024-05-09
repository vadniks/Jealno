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

#include "CompoundShader.hpp"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

CompoundShader::CompoundShader(const std::string& vertexCode, const std::string& fragmentCode) {
    int success;
    unsigned vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, (const char*[1]) {vertexCode.c_str()}, nullptr);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    assert(success == GL_TRUE);

    unsigned fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, (const char*[1]) {fragmentCode.c_str()}, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    assert(success == GL_TRUE);

    mProgramId = glCreateProgram();
    glAttachShader(mProgramId, vertex);
    glAttachShader(mProgramId, fragment);
    glLinkProgram(mProgramId);
    glGetProgramiv(mProgramId, GL_LINK_STATUS, &success);
    assert(success == GL_TRUE);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

CompoundShader::~CompoundShader() {
    glDeleteProgram(mProgramId);
}

void CompoundShader::use() {
    glUseProgram(mProgramId);
}

void CompoundShader::setValue(const std::string& name, bool value) {
    glUniform1i(glGetUniformLocation(mProgramId, name.c_str()), (int) value);
}

void CompoundShader::setValue(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(mProgramId, name.c_str()), value);
}

void CompoundShader::setValue(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(mProgramId, name.c_str()), value);
}

void CompoundShader::setValue(const std::string& name, const glm::vec2& value) {
    glUniform2f(glGetUniformLocation(mProgramId, name.c_str()), value.x, value.y);
}

void CompoundShader::setValue(const std::string& name, const glm::vec3& value) {
    glUniform3f(glGetUniformLocation(mProgramId, name.c_str()), value.x, value.y, value.z);
}

void CompoundShader::setValue(const std::string& name, const glm::vec4& value) {
    glUniform4f(glGetUniformLocation(mProgramId, name.c_str()), value.x, value.y, value.z, value.w);
}

void CompoundShader::setValue(const std::string& name, const glm::mat3& value) {
    glUniformMatrix3fv(glGetUniformLocation(mProgramId, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void CompoundShader::setValue(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(glGetUniformLocation(mProgramId, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
