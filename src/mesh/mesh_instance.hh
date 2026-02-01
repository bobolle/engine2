#ifndef _mesh_instance_hh_
#define _mesh_instance_hh_

#include <mesh/mesh.hh>
#include <shader/shader.hh>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class mesh_instance {
  public:
    mesh_instance() = default;
    mesh_instance(mesh* mesh_ptr, glm::vec3 position, glm::quat rotation, glm::vec3 scale);
    mesh* mesh_ptr;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    glm::mat4 get_model_matrix(void);
    void draw(shader& s);
};

#endif
