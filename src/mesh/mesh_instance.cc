#include <mesh/mesh_instance.hh>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

mesh_instance::mesh_instance(mesh* mesh_ptr, glm::vec3 position, glm::quat rotation, glm::vec3 scale)
  : mesh_ptr(mesh_ptr), position(position),
    rotation(rotation), scale(scale) {}

glm::mat4 mesh_instance::get_model_matrix(void) {
  glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
  model *= glm::mat4_cast(rotation);
  model = glm::scale(model, scale);
  return model;
}

void mesh_instance::draw(shader& s) {
  glUniformMatrix4fv(s.uniform_model, 1, GL_FALSE, glm::value_ptr(get_model_matrix()));
  mesh_ptr->draw();
}
