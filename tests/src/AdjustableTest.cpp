#include <gtest/gtest.h>

#include <Cellnta/Adjustable.h>

using namespace Cellnta;

class AdjustableTest : public Adjustable, public Iterable {
 public:
  IteratorRef CreateIterator() const {
    return std::make_unique<AdjustableTest::Iterator>(*this);
  }

  IteratorRef CreateIterator(const Area&) const { return nullptr; }

 protected:
  bool OnSetCell(const Cell& cell) {
    m_data.push_back(cell);
    return false;
  }

  Cell::State OnGetCell(const Cell::Pos& pos) const {
    auto res =
        std::find_if(m_data.begin(), m_data.end(), [&](const Cell& cell) {
          if (cell.pos.size() != pos.size())
            return false;
          return cell.pos == pos;
        });
    if (res == m_data.end())
      return 0;
    return res->state;
  }

 private:
  class Iterator : public Cellnta::Iterator {
   public:
    Iterator(const AdjustableTest& adjust) : m_adjust(adjust) { Reset(); }

    void Reset() override { m_iter = m_adjust.m_data.begin(); }

    const Cell* Next() override {
      if (m_iter == m_adjust.m_data.end())
        return nullptr;

      m_curr.pos = m_iter->pos;
      m_curr.state = m_iter->state;

      ++m_iter;
      return &m_curr;
    }

   private:
    const AdjustableTest& m_adjust;
    std::vector<Cell>::const_iterator m_iter;
    Cell m_curr;
  };

  std::vector<Cell> m_data;
};

TEST(Adjustable, SetCell) {
  AdjustableTest adjust;

  adjust.SetCell(Cell(Cell::Pos::Constant(1, 1), 1));
  ASSERT_EQ(adjust.GetCell(Cell::Pos::Constant(1, 1)), 1);

  adjust.SetCell(Cell::Pos::Constant(2, 2), 2);
  ASSERT_EQ(adjust.GetCell(Cell::Pos::Constant(2, 2)), 2);

  std::vector<Cell> cellList = {Cell(Cell::Pos::Constant(3, 3), 3),
                                Cell(Cell::Pos::Constant(4, 4), 4),
                                Cell(Cell::Pos::Constant(5, 5), 5)};
  adjust.SetCell(cellList);
  for (const auto& expectedCell : cellList)
    ASSERT_EQ(adjust.GetCell(expectedCell.pos), expectedCell.state);

  AdjustableTest adjustBuffer;
  IteratorRef iter = adjust.CreateIterator();
  adjustBuffer.SetCell(adjust.CreateIterator());

  while (const Cell* cell = iter->Next()) {
    ASSERT_EQ(adjustBuffer.GetCell(cell->pos), cell->state);
  }
}
