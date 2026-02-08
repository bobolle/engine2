#include <entities/entity.hh>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::mat4 entity::get_model_matrix(void) {
  glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
  model *= glm::mat4_cast(rotation);
  model = glm::scale(model, scale);
  return model;
}

void entity::draw(shader& shader_ptr) {
  glUniformMatrix4fv(shader_ptr.uniform_model, 1, GL_FALSE, glm::value_ptr(get_model_matrix()));
  mesh_ptr->draw();
}
