#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include <Eigen/Core>

#define CHUNK_SIZE ((size_t)16)

namespace Cellnta {

typedef int32_t cell_t;
typedef uint8_t state_t;

struct Cell {
  using Vec = Eigen::Vector<cell_t, Eigen::Dynamic>;
  Cell() = default;
  Cell(const Vec& pos, state_t state) : pos(pos), state(state) {}
  Vec pos;
  state_t state;
};

template <typename T>
struct EigeneHasher {
  size_t operator()(T const& data) const {
    size_t seed = 0;
    for (size_t i = 0; i < data.size(); ++i)
      seed ^= std::hash<typename T::Scalar>()(data(i)) + 0x9e3779b9 +
              (seed << 6) + (seed >> 2);
    return seed;
  }
};

struct Area {
  Eigen::Vector3i min;
  Eigen::Vector3i max;

  Area() = default;
  Area(const Eigen::Vector3i& min, const Eigen::Vector3i& max);
  Area(const int min, const int max);

  bool Valid() const;
  bool CellValid(const Eigen::VectorXi& cell) const;
  size_t GetSize() const { return min.size(); }

  static Area Intersection(const Area& first, const Area& second);
  static std::vector<Area> InverseClip(const Area& clipper,
                                       const Area& subject);

  Area operator+(const Area& r) const { return Area(min + r.min, max + r.max); }

  Area operator+(const Eigen::Vector3i& r) const {
    return Area(min + r, max + r);
  }

  Area operator-(const Area& r) const { return Area(min - r.min, max - r.max); }

  Area operator-(const Eigen::Vector3i& r) const {
    return Area(min - r, max - r);
  }
};

class ChunkPos {
 public:
  ChunkPos() = default;
  ChunkPos(const Eigen::Vector3i& pos) { Set(pos); }
  ChunkPos(Eigen::Vector3i&& pos) { Set(std::move(pos)); }
  ChunkPos(int x, int y, int z) { Set(x, y, z); }

  void Set(const Eigen::Vector3i& pos) { m_pos = pos; }
  void Set(Eigen::Vector3i&& pos) { m_pos = std::move(pos); }
  void Set(int x, int y, int z) {
    m_pos.x() = x;
    m_pos.y() = y;
    m_pos.z() = z;
  }

  const Eigen::Vector3i& GetPosition() const { return m_pos; }
  Eigen::Vector3i GetPosition() { return m_pos; }

  int x() const { return m_pos.x(); }
  int y() const { return m_pos.y(); }
  int z() const { return m_pos.z(); }

  bool operator==(const ChunkPos& other) const {
    return (m_pos == other.m_pos);
  }
  bool operator!=(const ChunkPos& other) const { return !(*this == other); }
  ChunkPos operator+(const ChunkPos& r) const {
    return ChunkPos(m_pos + r.m_pos);
  }

 private:
  Eigen::Vector3i m_pos;
};

struct ChunkPosHasher {
  size_t operator()(const ChunkPos& k) const {
    EigeneHasher<Eigen::Vector3i> hasher;
    return hasher(k.GetPosition());
  }
};

class Chunk {
 public:
  using Map = std::unordered_map<Eigen::VectorXi, state_t,
                                 EigeneHasher<Eigen::VectorXi>>;

  Chunk(size_t dim, const ChunkPos& pos) : m_d(dim), m_pos(pos) {}
  ~Chunk() = default;

  void SetCell(const Eigen::VectorXi& pos, state_t state);

  const Map& GetData() const { return m_data; }
  ChunkPos GetPosition() const { return m_pos; }
  size_t GetDimensions() const { return m_d; }

  void Clear();

  void Handled() { m_needUpdate = false; }
  bool NeedUpdate() { return m_needUpdate; }

  static ChunkPos GetPosFromGlobalPos(const Eigen::VectorXf& pos) {
    return ChunkPos(Eigen::Vector3i(pos.x(), pos.y(), pos.z()) / CHUNK_SIZE);
  }

  Map::iterator begin() { return m_data.begin(); }
  Map::const_iterator begin() const { return m_data.begin(); }
  Map::iterator end() { return m_data.end(); }
  Map::const_iterator end() const { return m_data.end(); }

 private:
  size_t m_d;
  ChunkPos m_pos;
  Map m_data;
  bool m_needUpdate = false;
};

class RenderData {
 public:
  using ChunkPtr = std::shared_ptr<Chunk>;
  using Map = std::unordered_map<ChunkPos, ChunkPtr, ChunkPosHasher>;

  RenderData();
  RenderData(size_t dim);

  void SetDimensions(size_t dim) {
    m_data.clear();
    m_d = dim;
  }

  void SetChunk(const ChunkPtr& chunk);
  void SetCell(const Eigen::VectorXi& pos, state_t state);

  const Map& GetData() const;
  ChunkPtr GetChunk(const ChunkPos& pos);
  void Clear();

  void Handled() { m_needUpdate = false; }
  bool NeedUpdate() const { return m_needUpdate; }

  int GetCollatingX() const { return m_collX; }
  int GetCollatingY() const { return m_collY; }
  int GetCollatingZ() const { return m_collZ; }

  void SetCollatingX(int x) {
    m_collX = x;
    m_desiredArea.push_back(GetDistanceArea());
  }
  void SetCollatingY(int y) {
    m_collY = y;
    m_desiredArea.push_back(GetDistanceArea());
  }
  void SetCollatingZ(int z) {
    m_collZ = z;
    m_desiredArea.push_back(GetDistanceArea());
  }

  void SetDistance(size_t distance);
  uint32_t GetDistance() const { return m_distance; }
  size_t GetArea() const { return m_distance * m_distance * m_distance; }

  void SetPosition(const ChunkPos& pos);

  const std::vector<Area>& GetDesireArea() const { return m_desiredArea; }
  bool DesireArea() const { return !m_desiredArea.empty(); }
  void DesireAreaProcessed() { m_desiredArea.clear(); }

  Map::iterator begin() { return m_data.begin(); }
  Map::const_iterator begin() const { return m_data.begin(); }
  Map::iterator end() { return m_data.end(); }
  Map::const_iterator end() const { return m_data.end(); }

 private:
  void UpdateVisibleArea(const Eigen::Vector3i& newPos);
  void EraseUnvisibleArea(const ChunkPos& oldPos, const ChunkPos& newPos);

  bool ChunkValid(const ChunkPos& pos);

  Area GetDistanceArea() const {
    const int cellDistance = m_distance * CHUNK_SIZE;
    return Area(-cellDistance, cellDistance);
  }

  Area GetDistanceChunkArea() const {
    const int chunkDistance = m_distance;
    return Area(-chunkDistance, chunkDistance);
  }

  size_t m_d = 0;
  Map m_data;
  bool m_needUpdate = false;
  size_t m_distance = 0;
  ChunkPos m_pos = ChunkPos(Eigen::Vector3i(0, 0, 0));
  Area m_visibleArea = GetDistanceArea();

  std::vector<Area> m_desiredArea;

  int m_collX = 0;
  int m_collY = 1;
  int m_collZ = 2;
};

}  // namespace Cellnta
