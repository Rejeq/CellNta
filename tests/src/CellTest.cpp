#include <gtest/gtest.h>

#include <Cellnta/Cell.h>

using namespace Cellnta;

TEST(Cell, Initialization) {
  Cell cell;
  EXPECT_TRUE(cell.pos.isZero());
  EXPECT_EQ(cell.state, 0);

  cell = Cell(Eigen::Vector3i::Zero(), 25);
  EXPECT_TRUE(cell.pos.isZero());
  EXPECT_EQ(cell.state, 25);

  cell = Cell(Eigen::Vector3i::Constant(10), 25);
  EXPECT_EQ(cell.pos, Eigen::Vector3i::Constant(10));
  EXPECT_EQ(cell.state, 25);
}

