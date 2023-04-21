#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Cellnta/Adjustable.h>

#include "FakeIterator.h"

using ::testing::_;
using namespace Cellnta;

class MockAdjustable : public Adjustable {
 public:
  MOCK_METHOD(bool, OnSetCell, (const Cell&), (override));
  MOCK_METHOD(Cell::State, OnGetCell, (const Cell::Pos&), (const, override));
};

TEST(Adjustable, SetCell) {
  MockAdjustable adjust;
  EXPECT_CALL(adjust, OnSetCell(_)).Times(2);

  adjust.SetCell(Cell(Cell::Pos::Constant(1, 1), 1));
  adjust.SetCell(Cell::Pos::Constant(20, 20), 20);
}

TEST(Adjustable, SetContainer) {
  std::vector<Cell> cells;
  cells.push_back(Cell(Cell::Pos::Constant(1, 1), 1));
  cells.push_back(Cell(Cell::Pos::Constant(2, 2), 2));
  cells.push_back(Cell(Cell::Pos::Constant(3, 3), 3));

  MockAdjustable adjust;
  EXPECT_CALL(adjust, OnSetCell(_)).Times(cells.size());

  adjust.SetCell(cells);
}

TEST(Adjustable, SetIterator) {
  FakeIterator iter;
  iter.Push(Cell(Cell::Pos::Constant(1, 1), 1));
  iter.Push(Cell(Cell::Pos::Constant(10, 10), 10));
  iter.Push(Cell(Cell::Pos::Constant(100, 100), 100));

  MockAdjustable adjust;
  EXPECT_CALL(adjust, OnSetCell(_)).Times(iter.Size());

  adjust.SetCell(iter);
}

TEST(Adjustable, GetCell) {
  MockAdjustable adjust;
  EXPECT_CALL(adjust, OnGetCell(_)).Times(1);

  adjust.GetCell(Cell::Pos::Constant(1, 1));
}
