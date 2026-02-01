#ifndef _mesh_hh_
#define _mesh_hh_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

struct vertex {
  glm::vec3 position;
  glm::vec3 color;
};

class mesh {
  public:
    std::vector<vertex> vertices;
    std::vector<unsigned int> indices;

    unsigned int vao, vbo, ebo;
    unsigned int indices_size;

    mesh(const std::vector<vertex>& vertices, const std::vector<unsigned int>& indices)
      : vertices(vertices), indices(indices) {
      indices_size = indices.size();

      glGenVertexArrays(1, &vao);
      glGenBuffers(1, &vbo);
      glGenBuffers(1, &ebo);

      glBindVertexArray(vao);

      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
      glEnableVertexAttribArray(0);

      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));
      glEnableVertexAttribArray(1);

      glBindVertexArray(0);
    }

    ~mesh() {
      glDeleteVertexArrays(1, &vao);
      glDeleteBuffers(1, &vbo);
      glDeleteBuffers(1, &ebo);
    }

    void draw(void) const {
      glBindVertexArray(vao);
      glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, 0); 
    }
};

#endif
