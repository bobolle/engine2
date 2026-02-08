#ifndef _entity_hh_
#define _entity_hh_

#include <mesh/mesh.hh>
#include <shader/shader.hh>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class entity {
  public:
    entity() = default;
    entity(mesh* mesh_ptr, glm::vec3 position, glm::quat rotation, glm::vec3 scale) :
      mesh_ptr(mesh_ptr), position(position), rotation(rotation), scale(scale) {}
    mesh* mesh_ptr;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    glm::mat4 get_model_matrix(void);
    void draw(shader& s);
};

#endif
