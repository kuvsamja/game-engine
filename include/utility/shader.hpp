#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <iterator>

#include <utility/vec2.hpp>

class Shader {
  public:
     GLint ID;
    
    /* constructor reads and builds the shader */
    Shader(const char* vertex_shader_path, const char* fragment_shader_path) {
        // reading shaders
        // // vertex
        std::ifstream vertex_shader_file_stream(vertex_shader_path);
        if(!vertex_shader_file_stream.is_open()) {
            std::cerr << "ERROR: opening vertex shader file failed" << std::endl;
            std::exit(-1);
        }
        std::string vertex_shader_text{
            std::istreambuf_iterator<char>(vertex_shader_file_stream),
            std::istreambuf_iterator<char>()
        };
        
        const char* vertex_shader_src = vertex_shader_text.c_str();

        // // fragment
        std::ifstream fragment_shader_file_stream(fragment_shader_path);
        if(!fragment_shader_file_stream.is_open()) {
            std::cerr << "ERROR: opening fragment shader file failed" << std::endl;
            std::exit(-1);
        }
        std::string fragment_shader_text{
            std::istreambuf_iterator<char>(fragment_shader_file_stream),
            std::istreambuf_iterator<char>()
        };


        const char* fragment_shader_src = fragment_shader_text.c_str();

        // compilation
        unsigned int vertex, fragment;
        int success;
        char info_log[512];

        // // vertex
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertex_shader_src, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(vertex, 512, NULL, info_log);
            std::cerr << "ERROR: vertex shader compilation failed\n" << info_log << std::endl;
            std::exit(-1);
        };

        // // fragment
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragment_shader_src, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(fragment, 512, NULL, info_log);
            std::cerr << "ERROR: fragment shader compilation failed\n" << info_log << std::endl;
            std::exit(-1);
        }

        // // program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success){
            glGetProgramInfoLog(ID, 512, NULL, info_log);
            std::cerr << "ERROR: shader program compilation failed\n" << info_log << std::endl;
            std::exit(-1);
        }

        // delete the linked shaders
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    ~Shader() {
        glDeleteProgram(ID);
    }

    void use() {
        glUseProgram(ID);
    }
    // utility uniform functions
    void setBool(const std::string &name, bool value) const {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    void setInt(const std::string &name, int value) const { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    void setFloat(const std::string &name, float value) const { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    void setVec2(const std::string &name, const glm::vec2 &value) const { 
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec2(const std::string &name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
    }
    void setVec2(const std::string &name, vec2<double> vec) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), vec.x(), vec.y()); 
    }
    void setVec3(const std::string &name, const glm::vec3 &value) const { 
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec3(const std::string &name, float x, float y, float z) const { 
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
    }
    void setVec4(const std::string &name, const glm::vec4 &value) const { 
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const { 
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
    }
    void setMat2(const std::string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat3(const std::string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    

};


// TODO: maybe use cpp file streams

#endif