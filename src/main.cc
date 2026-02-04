#include <common/window.hh>
#include <shader/shader.hh>
#include <camera/noclip_camera.hh>
#include <camera/fps_camera.hh>
#include <input.hh>

#include <mesh/mesh.hh>
#include <mesh/mesh_loader.hh>
#include <mesh/mesh_instance.hh>

#include <ray.hh>
#include <terrain/terrain.hh>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <vector>
#include <string>
#include <cmath>

int main(void) {
  glfw_window game_window(1024, 768, "game");
  GLFWwindow* window = game_window.handle();

  input_manager input;
  input.init(window);

  shader entity_shader;
  entity_shader.init("shaders/entity.vs", "shaders/entity.fs");

  fps_camera camera_fps(&game_window, &input.state, game_window.aspect());
  noclip_camera camera_noclip(&game_window, &input.state, game_window.aspect());

  game_window.set_resize_callback([&](float aspect) {
    camera_noclip.set_aspect(aspect); 
    camera_fps.set_aspect(aspect); 
  });

  mesh square_mesh = load_mesh_from_file("assets/square.obj");

  mesh_instance square_object(&square_mesh, glm::vec3(0.0f, 0.0f, 0.0f),
      glm::quat(glm::vec3(0.0f)),
      glm::vec3(1.0f, 1.0f, 1.0f));

  terrain_manager terrain;
  //terrain.load_all_chunks("world_data/");

  glm::mat4 view  = glm::mat4(1.0f);
  glm::mat4 projection  = glm::mat4(1.0f);

  constexpr float tick_rate = 60.0f;
  constexpr float tick_dt = (1.0f / tick_rate);
  float current_frame;
  float dt;
  float last_frame = 0.0f;
  float accumulator = 0.0f;

  const float chunk_world_size = units::chunk_size * units::tile_size;

  camera* current_camera = &camera_fps;

  while(!glfwWindowShouldClose(window)) {
    current_frame = (float)glfwGetTime();
    dt = current_frame - last_frame;
    last_frame = current_frame;

    if (dt > 0.25) { dt = 0.25; }
    accumulator += dt;

    // per tick update here
    while (accumulator >= tick_dt) {
      accumulator -= tick_dt;
    }

    float alpha = accumulator / tick_dt;

    current_camera->update(dt);
    view       = current_camera->get_view();
    projection = current_camera->get_projection();

    game_window.clear();
    entity_shader.use();

    // entities
    glUniformMatrix4fv(entity_shader.uniform_view, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(entity_shader.uniform_projection, 1, GL_FALSE, glm::value_ptr(projection));

    glm::ivec2 current_chunk_coord(
      static_cast<int>(std::floor(current_camera->position.x / chunk_world_size)),
      static_cast<int>(std::floor(current_camera->position.z / chunk_world_size))
    );

    terrain.update(current_chunk_coord);
    for (auto& [coord, instance] : terrain.chunk_instances) {
      instance.draw(entity_shader);
    }
    
    chunk* current_chunk = terrain.get_chunk(current_chunk_coord);
    float height = get_terrain_height(current_camera->position.x, current_camera->position.z, current_chunk);
    camera_fps.set_height(height + (2.5f * units::metre));
    std::string title = "x: " + std::to_string(std::floor(current_camera->position.x)) +
                       " z: " + std::to_string(std::floor(current_camera->position.z)) +
                       " terrain height: " + std::to_string(height);
    glfwSetWindowTitle(window, title.c_str());

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
