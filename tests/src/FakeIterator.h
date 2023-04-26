#pragma once

#include "Cellnta/Iterator.h"

using namespace Cellnta;

class FakeIterator : public IterBase::CellForward {
 public:
  FakeIterator() { Reset(); }

  void Reset() override { m_iter = m_data.begin(); }

  const Cell* Next() override {
    if (m_iter == m_data.end())
      return nullptr;

    m_curr = *m_iter;
    m_iter++;
    return &m_curr;
  }

  size_t Size() const { return m_data.size(); }

  void Push(const Cell& cell) {
    m_data.push_back(cell);
    Reset();
  }

  template <typename Container>
  void Push(const Container& cont) {
    m_data.insert(m_data.end(), cont.begin(), cont.end());
    Reset();
  }

 private:
  std::vector<Cell> m_data;
  std::vector<Cell>::iterator m_iter;
  Cell m_curr;
};
