#ifndef _player_hh_
#define _player_hh_

#include <entities/movable_entity.hh>

class player : public movable_entity {
  public:
    player() = default;
    player(mesh* mesh_ptr, glm::vec3 position, glm::quat rotation, glm::vec3 scale) : 
      movable_entity(mesh_ptr, position, rotation, scale) {} 
};

#endif
