#include <entities/movable_entity.hh>

void movable_entity::update(const float dt) {
  prev_position = position;

  position += velocity * dt;
}

glm::mat4 movable_entity::get_model_matrix(const float alpha) const {
  glm::vec3 render_position = glm::mix(prev_position, position, alpha);
  glm::mat4 model(1.0f);
  model = glm::translate(model, render_position);
  
  return model;
}
