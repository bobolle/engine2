#ifndef _shader_hh_
#define _shader_hh_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

class shader {
  public:
    shader() {}
    unsigned ID;
    int uniform_projection;
    int uniform_view;
    int uniform_model;

    void init(std::string vertex_path, std::string fragment_path);
    void use() { glUseProgram(this->ID); }
};

#endif
