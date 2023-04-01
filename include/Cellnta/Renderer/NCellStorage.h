#pragma once

#include <Eigen/Core>
#include <vector>

#include "Cellnta/Renderer/GlBackend.h"

namespace Cellnta {

class NCellStorage {
 public:
  using Point = float;
  using Vec = Eigen::Vector<Point, Eigen::Dynamic>;
  using VecList = std::vector<Vec>;

  NCellStorage();
  ~NCellStorage();

  // TODO: Different usage context with HypercubeStorage
  // In NCellStorage updated when within added cell
  // In HypercubeStorage updated when within changed gl buffer
  //
  // True only when cells changed
  bool NeedUpdate() const { return m_needUpdate; }
  void Handled() { m_needUpdate = false; }

  void Restore();

  void UpdateBuffer();

  void Add(const Vec& pos);
  void AddHomogeneous(const Vec& pos);

  void Erase(const VecList::iterator& pos);
  void Erase(const Vec& pos);
  void EraseHomogeneous(const Vec& pos);

  void SetDimension(int dim);

  void clear();
  void reserve(int capacity);
  size_t capacity() const;
  size_t size() const;

  int GetDimensions() const { return m_d; }

  GLuint GetBuffer() const { return m_buffer; }

  const VecList& GetRaw() const { return m_cells; }
  VecList& GetRaw() { return m_cells; }

  const VecList& GetVisibleRaw() const { return m_visibleCells; }
  VecList& GetVisibleRaw() { return m_visibleCells; }

 private:
  VecList m_cells;
  VecList m_visibleCells;

  GLuint m_buffer = 0;
  size_t m_oldCapacity = 0;
  bool m_needUpdate = false;
  int m_d = 0;
};

}  // namespace Cellnta
