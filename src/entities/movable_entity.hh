#ifndef _movable_entity_hh_
#define _movable_entity_hh_

#include <entities/entity.hh>

class movable_entity : public entity {
  public:
    movable_entity() = default;
    movable_entity(mesh* mesh_ptr, glm::vec3 position, glm::quat rotation, glm::vec3 scale) : 
      entity(mesh_ptr, position, rotation, scale),
      velocity(0.0f) {} 

    glm::vec3 velocity;

    void update(const float dt);
    glm::mat4 get_model_matrix(const float alpha) const override;

  private:
    glm::vec3 prev_position;
};

#endif
