#include <gtest/gtest.h>

#include <Cellnta/Snapshot.h>

using namespace Cellnta;

TEST(Snapshot, SetCell) {
  Snapshot snap;

  snap.SetDimension(0);
  snap.SetCell(Cell::Pos::Constant(0, 5), 5);
  ASSERT_EQ(snap.GetCell(Cell::Pos::Constant(98, 5)), 0);

  snap.SetDimension(25);
  snap.SetCell(Cell::Pos::Constant(25, 25), 25);
  ASSERT_EQ(snap.GetCell(Cell::Pos::Constant(25, 25)), 25);

  snap.SetCell(Cell::Pos::Constant(3, 3), 3);
  ASSERT_EQ(snap.GetCell(Cell::Pos::Constant(3, 3)), 0);
}

TEST(Snapshot, SetDimension) {
  Snapshot snap;

  snap.SetDimension(3);
  snap.SetCell(Cell::Pos::Constant(3, 3), 3);
  snap.SetDimension(4);
  ASSERT_EQ(snap.GetCell(Cell::Pos::Constant(3, 3)), 0);
}

TEST(Snapshot, Iterator) {
  const std::vector<Cell> expectedCells = {
      Cell(Eigen::Vector3i(1, 1, 1), 1), Cell(Eigen::Vector3i(2, 2, 2), 2),
      Cell(Eigen::Vector3i(3, 3, 3), 3), Cell(Eigen::Vector3i(4, 4, 4), 4),
      Cell(Eigen::Vector3i(5, 5, 5), 5), Cell(Eigen::Vector3i(6, 6, 6), 6),
      Cell(Eigen::Vector3i(7, 7, 7), 7), Cell(Eigen::Vector3i(8, 8, 8), 8),
      Cell(Eigen::Vector3i(9, 9, 9), 9), Cell(Eigen::Vector3i(10, 10, 10), 10),
  };
  Snapshot snap;
  snap.SetDimension(3);
  snap.SetCell(expectedCells);

  auto iter = snap.CreateIterator();
  int i = 0;

  while (const Cell* cell = iter->Next()) {
    ASSERT_EQ(*cell, expectedCells[i]);
    ++i;
  }
  if (i == 0)
    ASSERT_TRUE(false) << "Iterator no have any value";
}

TEST(Snapshot, AreaIterator) {
  const std::vector<Cell> expectedCells = {
      Cell(Eigen::Vector3i(1, 1, 1), 1), Cell(Eigen::Vector3i(2, 2, 2), 2),
      Cell(Eigen::Vector3i(3, 3, 3), 3), Cell(Eigen::Vector3i(4, 4, 4), 4),
      Cell(Eigen::Vector3i(5, 5, 5), 5), Cell(Eigen::Vector3i(6, 6, 6), 6),
      Cell(Eigen::Vector3i(7, 7, 7), 7), Cell(Eigen::Vector3i(8, 8, 8), 8),
      Cell(Eigen::Vector3i(9, 9, 9), 9), Cell(Eigen::Vector3i(10, 10, 10), 10),
  };
  Snapshot snap;
  snap.SetDimension(3);
  snap.SetCell(expectedCells);

  auto iter = snap.CreateIterator(Area(3, 8));
  int i = 2;

  while (const Cell* cell = iter->Next()) {
    ASSERT_EQ(*cell, expectedCells[i]);
    ++i;
  }
  if (i != 7)
    ASSERT_TRUE(false) << "Iterator did not go through all the values";

  iter = snap.CreateIterator(Area(-1234, 1234));
  i = 0;

  while (const Cell* cell = iter->Next()) {
    ASSERT_EQ(*cell, expectedCells[i]);
    ++i;
  }
  if (i != 10)
    ASSERT_TRUE(false) << "Iterator did not go through all the values";

  iter = snap.CreateIterator(Area(4321, -128));
  ASSERT_FALSE(iter->Next()) << "Iterator contain value, but area is invalid";
}
