#ifndef _terrain_hh_
#define _terrain_hh_

#include <common/units.hh>
#include <mesh/mesh.hh>
#include <mesh/mesh_instance.hh>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <regex>
#include <vector>
#include <string>
#include <fstream>
#include <functional>

struct ivec2_hash {
  std::size_t operator() (const glm::ivec2& v) const noexcept {
    std::hash<int> hasher;
    size_t h1 = hasher(v.x);
    size_t h2 = hasher(v.y);

    return h1 ^ (h2 << 1);
  }
};

enum tile_type {
  grass,
  dirt,
  stone,
  none
};

struct tile {
  float h00, h10, h01, h11;
  tile_type type;
  glm::vec3 color;
};

struct tile_disk {
  float h00, h10, h01, h11;
  uint8_t type;
  float color[3];
};

struct chunk_disk {
  glm::ivec2 chunk_coord;
  float heights[units::chunk_size + 1][units::chunk_size + 1];
  tile_disk tiles[units::chunk_size][units::chunk_size];
};

struct chunk {
  glm::ivec2 chunk_coord;
  float heights[units::chunk_size + 1][units::chunk_size + 1];
  tile tiles[units::chunk_size][units::chunk_size];
};

class terrain_manager {
  public:
    terrain_manager(void) = default;
    ~terrain_manager(void) = default;

    void load_chunk(const glm::ivec2& chunk_coord);
    bool load_chunk_from_file(const std::filesystem::path& path);
    void load_all_chunks(const std::string& path);
    chunk* get_chunk(const glm::ivec2& chunk_coord);
    void unload_chunk(const glm::ivec2& chunk_coord);

    std::unordered_map<glm::ivec2, chunk, ivec2_hash> loaded_chunks;
    std::unordered_map<glm::ivec2, std::unique_ptr<mesh>, ivec2_hash> chunk_meshes;
    std::unordered_map<glm::ivec2, mesh_instance, ivec2_hash> chunk_instances;
};

chunk create_chunk(glm::ivec2 chunk_coord);
std::unique_ptr<mesh> create_mesh_from_chunk(chunk& chunk_ptr);
float sample_height(float world_x, float world_z, chunk* chunk_ptr);

#endif
