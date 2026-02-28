#include <common/window.hh>
#include <shader/shader.hh>
#include <camera/noclip_camera.hh>
#include <camera/fps_camera.hh>
#include <input.hh>
#include <mesh/mesh.hh>
#include <mesh/mesh_loader.hh>
#include <entities/entity.hh>
#include <entities/movable_entity.hh>
#include <entities/player.hh>
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
  // create some window stuff
  glfw_window game_window(1024, 768, "game");
  GLFWwindow* window = game_window.handle();

  // create someone to manage our inputs
  input_manager input;
  input.init(window);

  // load some shaders
  shader entity_shader;
  entity_shader.init("shaders/entity.vs", "shaders/entity.fs");

  // temporary two seperate camera objects here for switching between FPP and TPP
  fps_camera camera_fps(&game_window, &input.state, game_window.aspect());
  noclip_camera camera_noclip(&game_window, &input.state, game_window.aspect());

  // set the bs callback for resizing window
  game_window.set_resize_callback([&](float aspect) {
    camera_noclip.set_aspect(aspect); 
    camera_fps.set_aspect(aspect); 
  });

  // temporary square mech for testing stuff
  mesh square_mesh = load_mesh_from_file("assets/square.obj");

  // create a movable from the square
  movable_entity square_object(&square_mesh, glm::vec3(0.0f, -5.0f, 0.0f),
      glm::quat(glm::vec3(0.0f)),
      glm::vec3(1.0f, 1.0f, 1.0f));

  // create someone that can load and manage the terrain
  terrain_manager terrain;

  // some stuff that I need to init before rendering a frame
  glm::mat4 view  = glm::mat4(1.0f);
  glm::mat4 projection  = glm::mat4(1.0f);

  // tick stuff
  constexpr float tick_rate = 60.0f;
  constexpr float tick_dt = (1.0f / tick_rate);
  float current_frame;
  float dt;
  float last_frame = 0.0f;
  float accumulator = 0.0f;

  // select which camera to use
  //camera* current_camera = &camera_noclip;
  camera* current_camera = &camera_fps;

  // main loop
  while(!glfwWindowShouldClose(window)) {
    current_frame = (float)glfwGetTime();
    dt = current_frame - last_frame;
    last_frame = current_frame;

    if (dt > 0.25) { dt = 0.25; }
    accumulator += dt;

    // per tick update here
    while (accumulator >= tick_dt) {
      square_object.update(tick_dt);
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

    chunk* current_chunk = terrain.get_chunk({ current_camera->position.x, current_camera->position.z });
    chunk* current_square_chunk = terrain.get_chunk({ square_object.position.x, square_object.position.z });

    float height = get_terrain_height(current_camera->position.x, current_camera->position.z, current_chunk);
    float square_height = get_terrain_height(square_object.position.x, square_object.position.z, current_square_chunk);

    square_object.velocity = glm::vec3(3.0f, 0.0f, 3.0f);
    square_object.position.y = square_height + 0.2f;
    square_object.draw(entity_shader, alpha);

    // render terrain chunks
    terrain.update({ current_camera->position.x, current_camera->position.z });
    for (auto& [coord, instance] : terrain.chunk_instances) {
      instance.draw(entity_shader, alpha);
    }
    
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
