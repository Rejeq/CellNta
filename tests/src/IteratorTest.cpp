#include <gtest/gtest.h>
#include <vector>

#include <Cellnta/Area.h>
#include <Cellnta/Iterator.h>

using namespace Cellnta;

class IteratorTest : public Cellnta::Iterator {
 public:
  IteratorTest(const std::vector<Cell>& cells) : m_cells(cells) { Reset(); }

  void Reset() override { m_idx = 0; }

  const Cell* Next() override {
    if (m_idx >= m_cells.size())
      return nullptr;

    m_curr.pos = m_cells[m_idx].pos;
    m_curr.state = m_cells[m_idx].state;

    ++m_idx;
    return &m_curr;
  }

 private:
  std::vector<Cell> m_cells;
  size_t m_idx = 0;
  Cell m_curr;
};

TEST(Iterator, Nth) {
  std::vector<Cell> expectedCells = {
      Cell(Cell::Pos::Constant(1, 1), 1), Cell(Cell::Pos::Constant(2, 2), 2),
      Cell(Cell::Pos::Constant(3, 3), 3), Cell(Cell::Pos::Constant(4, 4), 4),
      Cell(Cell::Pos::Constant(5, 5), 5)};

  IteratorTest iter = expectedCells;

  auto ValidateNth = [&](int nthIdx, int i) {
    iter.Nth(nthIdx);
    while (const Cell* cell = iter.Next()) {
      ASSERT_EQ(cell->state, expectedCells[i].state);
      i++;
    }
    iter.Reset();
  };

  ValidateNth(3, 3);
  ValidateNth(-123, 0);

  iter.Nth(123);
  ASSERT_EQ(iter.Next(), nullptr);
}
