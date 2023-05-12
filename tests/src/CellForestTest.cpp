#include <gtest/gtest.h>

#include <Cellnta/CellForest.h>

#include "Utils.h"

using namespace Cellnta;

static void SetContainer(CellForest<>& tree, const std::vector<Cell>& container) {
  for (const auto& cell : container)
    tree.Set(cell.pos, cell.state);
}

TEST(CellForest, Find) {
  CellForest tree;

  tree.Set(Cell::Pos::Constant(3, 255), 255);
  EXPECT_EQ(tree.Find(Cell::Pos::Constant(3, 255)), 255);

  tree.Set(Cell::Pos::Constant(2, 22), 22);
  EXPECT_EQ(tree.Find(Cell::Pos::Constant(2, 22)), 22);

  tree.Set(Cell::Pos::Constant(1, 1), 1);
  EXPECT_EQ(tree.Find(Cell::Pos::Constant(1, 1)), 1);
}

TEST(CellForest, FindNegativePos) {
  CellForest tree;

  tree.Set(Cell::Pos::Constant(3, -255), 255);
  EXPECT_EQ(tree.Find(Cell::Pos::Constant(3, -255)), 255);

  tree.Set(Cell::Pos::Constant(2, -22), 22);
  EXPECT_EQ(tree.Find(Cell::Pos::Constant(2, -22)), 22);

  tree.Set(Cell::Pos::Constant(1, -1), 1);
  EXPECT_EQ(tree.Find(Cell::Pos::Constant(1, -1)), 1);
}

TEST(CellForest, FindOverlap) {
  CellForest tree;

  tree.Set(Cell::Pos::Constant(3, 1), 3);
  tree.Set(Cell::Pos::Constant(2, 1), 2);
  tree.Set(Cell::Pos::Constant(1, 1), 1);

  EXPECT_EQ(tree.Find(Cell::Pos::Constant(3, 1)), 3);
  EXPECT_EQ(tree.Find(Cell::Pos::Constant(2, 1)), 2);
  EXPECT_EQ(tree.Find(Cell::Pos::Constant(2, 1)), 2);
}

TEST(CellForest, FindInvalid) {
  CellForest tree;

  tree.Set(Cell::Pos(), 1);
  EXPECT_EQ(tree.Find(Cell::Pos()), 0);
}

TEST(CellForest, Unset) {
  CellForest tree;

  tree.Set(Cell::Pos::Constant(3, 23), 23);
  ASSERT_EQ(tree.Find(Cell::Pos::Constant(3, 23)), 23);

  tree.Set(Cell::Pos::Constant(3, 23), 0);
  EXPECT_EQ(tree.Find(Cell::Pos::Constant(3, 23)), 0);
}

TEST(CellForest, FindUndefined) {
  CellForest tree;

  ASSERT_EQ(tree.Find(Cell::Pos::Constant(3, 33)), 0);
  ASSERT_EQ(tree.Find(Cell::Pos::Constant(2, 22)), 0);
  ASSERT_EQ(tree.Find(Cell::Pos()), 0);
}

TEST(CellForest, Size) {
  CellForest tree;

  for (int i = 1; i <= 4; ++i) {
    tree.Set(Cell::Pos::Constant(i, i), 1);
    ASSERT_EQ(tree.Size(), i) << "Current i is: '" << i << "'";
  }
}

TEST(CellForest, SizeEmpty) {
  CellForest tree;

  ASSERT_EQ(tree.Size(), 0);
}

TEST(CellForest, SizeWithUnset) {
  CellForest tree;
  constexpr int max = 4;

  for (int i = 1; i <= max; ++i) {
    tree.Set(Cell::Pos::Constant(i, i), 1);
    ASSERT_EQ(tree.Size(), i) << "Current i is: '" << i << "'";
  }

  for (int i = max; i >= 1; --i) {
    tree.Set(Cell::Pos::Constant(i, i), 0);
    ASSERT_EQ(tree.Size(), i - 1) << "Current i is: '" << i << "'";
  }
}

TEST(CellForest, SizeWithOverlap) {
  CellForest tree;

  for (int i = 1; i <= 4; ++i) {
    tree.Set(Cell::Pos::Constant(3, 3), i);
    ASSERT_EQ(tree.Size(), 1) << "Current i is: '" << i << "'";
  }
}

TEST(CellForest, Capacity) {
  CellForest tree;

  int expCapacity = 0;
  for (int i = 1; i <= 4; ++i) {
    expCapacity += i;

    tree.Set(Cell::Pos::Constant(i, i), 1);
    ASSERT_EQ(tree.Capacity(), expCapacity) << "Current i is: '" << i << "'";
  }
}

TEST(CellForest, CapacityEmpty) {
  CellForest tree;

  ASSERT_EQ(tree.Capacity(), 0);
}

TEST(CellForest, CapacityWithUnset) {
  CellForest tree;
  constexpr int max = 4;

  int expCapacity = 0;
  for (int i = 1; i <= max; ++i) {
    expCapacity += i;

    tree.Set(Cell::Pos::Constant(i, i), 1);
    ASSERT_EQ(tree.Capacity(), expCapacity) << "Current i is: '" << i << "'";
  }

  for (int i = 4; i >= 1; --i) {
    tree.Set(Cell::Pos::Constant(i, i), 0);
    ASSERT_EQ(tree.Capacity(), expCapacity) << "Current i is: '" << i << "'";
  }
}

TEST(CellForest, CapacityWithOverlap) {
  CellForest tree;

  for (int i = 1; i <= 4; ++i) {
    tree.Set(Cell::Pos::Constant(3, 3), i);
    ASSERT_EQ(tree.Capacity(), 3) << "Current i is: '" << i << "'";
  }
}

TEST(CellForest, Erase) {
  CellForest tree;

  for (int i = 1; i <= 4; ++i) {
    Cell::Pos pos = Cell::Pos::Constant(i, i);
    tree.Set(pos, i);

    int sizeBeforeErase = tree.Size();
    tree.Erase(pos);

    ASSERT_EQ(tree.Find(pos), 0);
    ASSERT_EQ(tree.Size(), sizeBeforeErase - 1);
  }
}

template<typename T>
static void CheckEraseArea(CellForest<T>& tree, int sizeBeforeErase,
                    const std::vector<Cell>& cells,
                    const std::vector<Cell>& erasedCells) {
  ASSERT_EQ(tree.Size(), sizeBeforeErase - erasedCells.size());

  for (const auto& cell : cells) {
    Cell::State expState = cell.state;

    auto res = std::find(erasedCells.begin(), erasedCells.end(), cell);
    if (res != erasedCells.end()) {
      expState = 0;
    }

    ASSERT_EQ(tree.Find(cell.pos), expState);
  }
}

TEST(CellForest, EraseArea) {
  CellForest tree;
  const Area area = Area(4, 7);
  const auto cells = GenerateCellList(Area(1, 10));
  auto erasedCells = GenerateCellList(area);

  SetContainer(tree, cells);

  int sizeBeforeErase = tree.Size();
  tree.EraseArea(area);

  CheckEraseArea(tree, sizeBeforeErase, cells, erasedCells);
}

TEST(CellForest, EraseInvalidArea) {
  CellForest tree;
  const Area area = Area(50, 0);
  const auto cells = GenerateCellList(Area(1, 10));
  auto erasedCells = std::vector<Cell>();

  SetContainer(tree, cells);

  int sizeBeforeErase = tree.Size();
  tree.EraseArea(area);

  CheckEraseArea(tree, sizeBeforeErase, cells, erasedCells);
}

TEST(CellForest, EraseOverOutOfBoundsArea) {
  CellForest tree;
  const Area area = Area(40, 50);
  const auto cells = GenerateCellList(Area(1, 10));
  auto erasedCells = std::vector<Cell>();

  SetContainer(tree, cells);

  int sizeBeforeErase = tree.Size();
  tree.EraseArea(area);

  CheckEraseArea(tree, sizeBeforeErase, cells, erasedCells);
}

TEST(CellForest, EraseUnderOutOfBoundsArea) {
  CellForest tree;
  const Area area = Area(-50, -40);
  const auto cells = GenerateCellList(Area(1, 10));
  auto erasedCells = std::vector<Cell>();

  SetContainer(tree, cells);

  int sizeBeforeErase = tree.Size();
  tree.EraseArea(area);

  CheckEraseArea(tree, sizeBeforeErase, cells, erasedCells);
}

TEST(CellForest, EraseLargeArea) {
  CellForest tree;
  const Area area = Area(-50, 50);
  const auto cells = GenerateCellList(Area(1, 10));
  auto erasedCells = cells;

  SetContainer(tree, cells);

  int sizeBeforeErase = tree.Size();
  tree.EraseArea(area);

  CheckEraseArea(tree, sizeBeforeErase, cells, erasedCells);
}

TEST(CellForest, EraseAreaPartiallyOverBounds) {
  CellForest tree;
  const Area area = Area(8, 20);
  const auto cells = GenerateCellList(Area(1, 10));
  auto erasedCells = GenerateCellList(Area(8, 10));

  SetContainer(tree, cells);

  int sizeBeforeErase = tree.Size();
  tree.EraseArea(area);

  CheckEraseArea(tree, sizeBeforeErase, cells, erasedCells);
}

TEST(CellForest, Clear) {
  CellForest tree;
  const Area area = Area(4, 7);
  const auto cells = GenerateCellList(Area(1, 10));
  auto expCells = GenerateCellList(area);

  SetContainer(tree, cells);
  tree.Clear();

  ASSERT_EQ(tree.Size(), 0);
  ASSERT_EQ(tree.Capacity(), 0);
}

TEST(CellForest, Move) {
  CellForest tree;

  Cell::Pos posFrom = Cell::Pos::Constant(3, 255);
  Cell::Pos posTo = Cell::Pos::Constant(2, 255);

  tree.Set(posFrom, 255);
  tree.Move(posFrom, posTo);

  ASSERT_EQ(tree.Find(posFrom), 0);
  ASSERT_EQ(tree.Find(posTo), 255);
}

TEST(CellForest, MoveEmpty) {
  CellForest tree;

  Cell::Pos posFrom = Cell::Pos::Constant(3, 255);
  Cell::Pos posTo = Cell::Pos::Constant(2, 255);

  tree.Move(posFrom, posTo);

  ASSERT_EQ(tree.Find(posFrom), 0);
  ASSERT_EQ(tree.Find(posTo), 0);
}

static void CheckAndDeleteCell(const Cell& cell, int min, int max,
                               std::vector<Cell>& expCells) {
  for (int i = 0; i < cell.pos.size(); ++i) {
    ASSERT_TRUE(Axis::WithinBound(min, max, cell.pos[i]))
        << "Cell is not in range: expcted [" << min << ", " << max << "] "
        << "but actual: " << cell.pos[i];
  }

  auto res = std::find(expCells.begin(), expCells.end(), cell);
  ASSERT_TRUE(res != expCells.end());
  expCells.erase(res);
}

TEST(CellForest, WholeIter) {
  CellForest tree;
  const Area area = Area(1, 10);
  auto expCells = GenerateCellList(area);

  SetContainer(tree, expCells);
  auto iter = tree.MakeWholeIter();

  while (const Cell* cell = iter.Next()) {
    CheckAndDeleteCell(*cell, area.MinAxis(0), area.MaxAxis(0), expCells);
  }

  ASSERT_TRUE(expCells.empty()) << "Iterator did not go through all the values";
}

TEST(CellForest, ValidAreaIter) {
  CellForest tree;
  const Area area = Area(4, 7);
  const auto cells = GenerateCellList(Area(1, 10));
  auto expCells = GenerateCellList(area);

  SetContainer(tree, cells);
  auto iter = tree.MakeAreaIter(area);

  while (const Cell* cell = iter.Next()) {
    CheckAndDeleteCell(*cell, area.MinAxis(0), area.MaxAxis(0), expCells);
  }
  ASSERT_TRUE(expCells.empty()) << "Iterator did not go through all the values";
}

TEST(CellForest, NegativeAreaIter) {
  CellForest tree;
  const Area area = Area(-200, -150);
  const auto cells = GenerateCellList(Area(-256, -128));
  auto expCells = GenerateCellList(area);

  SetContainer(tree, cells);
  auto iter = tree.MakeAreaIter(area);

  while (const Cell* cell = iter.Next()) {
    CheckAndDeleteCell(*cell, area.MinAxis(0), area.MaxAxis(0), expCells);
  }
  ASSERT_TRUE(expCells.empty()) << "Iterator did not go through all the values";
}

TEST(CellForest, OutOfRangeAreaIter) {
  CellForest tree;
  const Area area = Area(-128, 128);
  const auto cells = GenerateCellList(Area(1, 10));
  auto expCells = cells;

  SetContainer(tree, cells);
  auto iter = tree.MakeAreaIter(area);

  while (const Cell* cell = iter.Next()) {
    CheckAndDeleteCell(*cell, area.MinAxis(0), area.MaxAxis(0), expCells);
  }
  ASSERT_TRUE(expCells.empty()) << "Iterator did not go through all the values";
}

TEST(CellForest, InvalidAreaIter) {
  CellForest tree;
  const Area area = Area(128, -128);
  const auto cells = GenerateCellList(Area(1, 10));

  SetContainer(tree, cells);
  auto iter = tree.MakeAreaIter(area);
  ASSERT_FALSE(iter.Next()) << "Iterator contain value, but area is invalid";
}
