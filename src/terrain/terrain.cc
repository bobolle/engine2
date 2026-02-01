#include <terrain/terrain.hh>

void terrain_manager::load_chunk(const glm::ivec2& chunk_coord) {
}

bool terrain_manager::load_chunk_from_file(const std::filesystem::path& path) {
  std::regex chunk_file_regex(R"(chunk_(-?\d+)_(-?\d+)\.bin)");
  std::smatch match;
  std::string filename = path.filename().string();

  if (!std::regex_match(filename, match, chunk_file_regex)) {
    return false;
  }

  int cx = std::stoi(match[1]);
  int cz = std::stoi(match[2]);

  std::ifstream in(path, std::ios::binary);
  if (!in) {
    std::cerr << "failed to open chunk file: " << path << "\n";
    return false;
  }

  chunk_disk cd;

  in.read(reinterpret_cast<char*>(&cd), sizeof(cd));
  in.close();

  chunk chunk_tmp;
  chunk_tmp.chunk_coord = glm::ivec2(cx, cz);

  for (int z = 0; z <= units::chunk_size; ++z) {
    for (int x = 0; x <= units::chunk_size; ++x) {
      chunk_tmp.heights[z][x] = cd.heights[z][x];
    }
  }

  for (int z = 0; z < units::chunk_size; ++z) {
    for (int x = 0; x < units::chunk_size; ++x) {
      const tile_disk& td = cd.tiles[z][x];
      chunk_tmp.tiles[z][x].h00 = td.h00;
      chunk_tmp.tiles[z][x].h01 = td.h01;
      chunk_tmp.tiles[z][x].h10 = td.h10;
      chunk_tmp.tiles[z][x].h11 = td.h11;
      chunk_tmp.tiles[z][x].type = static_cast<tile_type>(td.type);
      chunk_tmp.tiles[z][x].color = glm::vec3(td.color[0], td.color[1], td.color[2]);
    }
  }

  loaded_chunks[glm::ivec2(cx, cz)] = chunk_tmp;
  chunk_meshes[chunk_tmp.chunk_coord] = create_mesh_from_chunk(chunk_tmp);

  mesh_instance instance(
      chunk_meshes[chunk_tmp.chunk_coord].get(),
      glm::vec3(chunk_tmp.chunk_coord.x * units::chunk_size * units::tile_size,
        0.0f,
        chunk_tmp.chunk_coord.y * units::chunk_size * units::tile_size),
      glm::quat(glm::vec3(0.0f)),
      glm::vec3(1.0f));

  chunk_instances[chunk_tmp.chunk_coord] = instance;

  std::cout << "loaded chunk_" << cx << "_" << cz << " into chunk_instances" << "\n";

  return true;
}

void terrain_manager::load_all_chunks(const std::string& path) {
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    if (!entry.is_regular_file()) continue;
    load_chunk_from_file(entry.path());
  }
}

chunk* terrain_manager::get_chunk(const glm::ivec2& chunk_coord) {
  auto it = loaded_chunks.find(chunk_coord);
  if (it != loaded_chunks.end()) {
    return &it->second;
  }
  return nullptr;
}

void terrain_manager::unload_chunk(const glm::ivec2& chunk_coord) {
  loaded_chunks.erase(chunk_coord);
}

chunk create_chunk(glm::ivec2 chunk_coord) {
  chunk chunk_tmp;
  chunk_tmp.chunk_coord = chunk_coord;

  for (int z = 0; z < units::chunk_size + 1; z++) {
    for (int x = 0; x < units::chunk_size + 1; x++) {
      int world_x = chunk_coord.x + x;
      int world_z = chunk_coord.y + z;
      chunk_tmp.heights[z][x] = sin(world_x / 2.0f) * sin(world_z / 2.0f) * 0.3f;
    }
  }

  for (int z = 0; z < units::chunk_size; z++) {
    for (int x = 0; x < units::chunk_size; x++) {
      chunk_tmp.tiles[z][x].type = stone;
    }
  }

  return chunk_tmp;
}

std::unique_ptr<mesh> create_mesh_from_chunk(chunk& chunk_ptr) {
  std::vector<vertex> vertices;
  std::vector<unsigned int> indices;

  for (int z = 0; z < units::chunk_size; z++) {
    for (int x = 0; x < units::chunk_size; x++) {
      float h00 = (chunk_ptr.heights[z][x] - 0.5f) * units::height_scale;
      float h01 = (chunk_ptr.heights[z][x + 1] - 0.5f) * units::height_scale;
      float h10 = (chunk_ptr.heights[z + 1][x] - 0.5f) * units::height_scale;
      float h11 = (chunk_ptr.heights[z + 1][x + 1] - 0.5f) * units::height_scale;

      float tx = x * units::tile_size;
      float tz = z * units::tile_size;

      glm::vec3 p00(tx, h00, tz);
      glm::vec3 p01((tx + units::tile_size), h01, tz);
      glm::vec3 p10(tx, h10, (tz + units::tile_size));
      glm::vec3 p11((tx+units::tile_size), h11, (tz+units::tile_size));

      tile_type type = chunk_ptr.tiles[z][x].type;
      glm::vec3 base_color;
      switch(type) {
        case grass: base_color = glm::vec3(0.2f, 0.7f, 0.3f); break;
        case dirt: base_color = glm::vec3(0.5f, 0.3f, 0.1f); break;
        case stone: base_color = glm::vec3(0.4f, 0.4f, 0.4f); break;
        case none: base_color = chunk_ptr.tiles[z][x].color;
      }

      float avg_height = (h00 + h01 + h10 + h11) / 4.0f;
      float factor = 0.5f + 0.05f * avg_height;
      glm::vec3 color = base_color * factor;
      //glm::vec3 color = base_color;

      unsigned int start_index = vertices.size();
      vertices.push_back({p00, color});
      vertices.push_back({p10, color});
      vertices.push_back({p01, color});
      vertices.push_back({p11, color});

      indices.push_back(start_index+0);
      indices.push_back(start_index+2);
      indices.push_back(start_index+1);
      indices.push_back(start_index+1);
      indices.push_back(start_index+2);
      indices.push_back(start_index+3);
    }
  }

  return std::make_unique<mesh>(vertices, indices);
}

float sample_height(float world_x, float world_z, chunk* chunk_ptr) {
  const float chunk_world_size = units::chunk_size * units::tile_size;

  float local_x = world_x - chunk_ptr->chunk_coord.x * chunk_world_size;
  float local_z = world_z - chunk_ptr->chunk_coord.y * chunk_world_size;

  int tile_x = static_cast<int>(local_x / units::tile_size);
  int tile_z = static_cast<int>(local_z / units::tile_size);

  tile_x = glm::clamp(tile_x, 0, units::chunk_size - 1);
  tile_z = glm::clamp(tile_z, 0, units::chunk_size - 1);

  float fx = (local_x - tile_x * units::tile_size) / units::tile_size;
  float fz = (local_z - tile_z * units::tile_size) / units::tile_size;

  fx = glm::clamp(fx, 0.0f, 1.0f);
  fz = glm::clamp(fz, 0.0f, 1.0f);

  float h00 = chunk_ptr->heights[tile_z][tile_x];
  float h01 = chunk_ptr->heights[tile_z][tile_x + 1];
  float h10 = chunk_ptr->heights[tile_z + 1][tile_x];
  float h11 = chunk_ptr->heights[tile_z + 1][tile_x + 1];

  float height;

  if (fx + fz <= 1.0f) {
    height = h00 * (1.0f - fx - fz) + h10 * fz + h01 * fx;
  } else {
    float ux = 1.0f - fx;
    float uz = 1.0f - fz;

    height = h11 * (1.0f - ux - uz) + h01 * uz + h10 * ux;
  }

  return height;
}
