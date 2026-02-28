#include <terrain/terrain.hh>

#include <unordered_set>

void terrain_manager::load_chunk(const glm::ivec2& chunk_coord) {
  if (loaded_chunks.find(chunk_coord) != loaded_chunks.end()) return;

  std::filesystem::path path =
    "world_data/chunk_" +
    std::to_string(chunk_coord.x) + "_" +
    std::to_string(chunk_coord.y) + ".bin";

  if (!std::filesystem::exists(path)) return;

  load_chunk_from_file(path);
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
  chunk_tmp.coord = glm::ivec2(cx, cz);

  for (int z = 0; z <= units::tiles_per_chunk; ++z) {
    for (int x = 0; x <= units::tiles_per_chunk; ++x) {
      chunk_tmp.heights[z][x] = cd.heights[z][x];
    }
  }

  for (int z = 0; z < units::tiles_per_chunk; ++z) {
    for (int x = 0; x < units::tiles_per_chunk; ++x) {
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
  chunk_meshes[chunk_tmp.coord] = create_mesh_from_chunk(chunk_tmp);

  entity instance(
      chunk_meshes[chunk_tmp.coord].get(),
      glm::vec3(chunk_tmp.coord.x * units::tiles_per_chunk * units::tile_size,
        0.0f,
        chunk_tmp.coord.y * units::tiles_per_chunk * units::tile_size),
      glm::quat(glm::vec3(0.0f)),
      glm::vec3(1.0f));

  chunk_instances[chunk_tmp.coord] = instance;

  return true;
}

void terrain_manager::load_all_chunks(const std::string& path) {
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    if (!entry.is_regular_file()) continue;
    load_chunk_from_file(entry.path());
  }
}

chunk* terrain_manager::get_chunk(const glm::ivec2& coord) {
  int x = static_cast<int>(std::floor(coord.x / units::chunk_size));
  int z = static_cast<int>(std::floor(coord.y / units::chunk_size));

  auto it = loaded_chunks.find({ x, z });
  if (it != loaded_chunks.end()) {
    return &it->second;
  }
  return nullptr;
}

void terrain_manager::unload_chunk(const glm::ivec2& chunk_coord) {
  loaded_chunks.erase(chunk_coord);
}

std::unique_ptr<mesh> create_mesh_from_chunk(chunk& chunk_ptr) {
  std::vector<vertex> vertices;
  std::vector<unsigned int> indices;

  for (int z = 0; z < units::tiles_per_chunk; z++) {
    for (int x = 0; x < units::tiles_per_chunk; x++) {
      float h00 = (chunk_ptr.heights[z][x]) * units::height_scale;
      float h01 = (chunk_ptr.heights[z][x + 1]) * units::height_scale;
      float h10 = (chunk_ptr.heights[z + 1][x]) * units::height_scale;
      float h11 = (chunk_ptr.heights[z + 1][x + 1]) * units::height_scale;

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
      float factor = 1.0f + (avg_height / units::height_scale);
      factor = glm::clamp(factor, 0.10f, 2.0f);

      glm::vec3 color = base_color * factor;

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

void terrain_manager::update(const glm::ivec2& update_coord) {
  std::unordered_set<glm::ivec2, ivec2_hash> request;
  
  glm::ivec2 coord(
    static_cast<int>(std::floor(update_coord.x / units::chunk_size)),
    static_cast<int>(std::floor(update_coord.y / units::chunk_size))
  );

  for (int z = -units::loaded_chunks_max; z <= units::loaded_chunks_max; ++z) {
    for (int x = -units::loaded_chunks_max; x <= units::loaded_chunks_max; ++x) {
      request.insert(coord + glm::ivec2(x, z));
    }
  }

  for (const glm::ivec2& chunk_coord : request) {
    if (loaded_chunks.find(chunk_coord) == loaded_chunks.end()) {
      load_chunk(chunk_coord);
    }
  }

  for (auto it = loaded_chunks.begin(); it != loaded_chunks.end();) {
    if (request.find(it->first) == request.end()) {
      chunk_meshes.erase(it->first);
      chunk_instances.erase(it->first);
      it = loaded_chunks.erase(it);
    } else {
      ++it;
    }
  }
}

float get_terrain_height(float world_x, float world_z, chunk* chunk_ptr) {
  if (!chunk_ptr) return -99.0f;

  const float chunk_world_size = units::tiles_per_chunk * units::tile_size;

  float local_x = world_x - chunk_ptr->coord.x * chunk_world_size;
  float local_z = world_z - chunk_ptr->coord.y * chunk_world_size;

  int tile_x = static_cast<int>(std::floor(local_x / units::tile_size));
  int tile_z = static_cast<int>(std::floor(local_z / units::tile_size));

  tile_x = glm::clamp(tile_x, 0, units::tiles_per_chunk - 1);
  tile_z = glm::clamp(tile_z, 0, units::tiles_per_chunk - 1);

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

  return height * units::height_scale;
}

glm::vec3 terrain_normal(float world_x, float world_z, chunk* chunk_ptr) {
  const float e = 0.1f;

  float hl = get_terrain_height(world_x - e, world_z, chunk_ptr);
  float hr = get_terrain_height(world_x + e, world_z, chunk_ptr);
  float hd = get_terrain_height(world_x, world_z - e, chunk_ptr);
  float hu = get_terrain_height(world_x, world_z + e, chunk_ptr);

  glm::vec3 n = glm::normalize(glm::vec3(
    hl - hr,
    2.0f * e,
    hd - hu
  ));

  return n;
}
