#include <gtest/gtest.h>

#include <Cellnta/Axis.h>
#include <Cellnta/World/Impl/Simple.h>

#include "Utils.h"

using namespace Cellnta;

static void InitWorld(WorldImplSimple& world, const std::vector<size_t>& size,
                      const std::vector<Cell> expected) {
  world.SetDimension(size.size());
  world.SetSize(size);

  ASSERT_EQ(world.GetDimension(), size.size());
  ASSERT_EQ(world.GetSize(), size);

  for (const auto& cell : expected)
    world.SetCell(cell);
}

TEST(WorldImplSimple, BlinkerGeneration) {
  int x = 15, y = 15, z = 15;

  // clang-format off
  const std::vector<Cell> step0 = {
    Cell(Eigen::Vector3i(x, y, z), 1),
    Cell(Eigen::Vector3i(x - 1, y, z), 1), Cell(Eigen::Vector3i(x + 1, y, z), 1),
    Cell(Eigen::Vector3i(x, y - 1, z), 1), Cell(Eigen::Vector3i(x, y + 1, z), 1),
    Cell(Eigen::Vector3i(x, y, z - 1), 1), Cell(Eigen::Vector3i(x, y, z + 1), 1),
 };

  const std::vector<Cell> step1 = {
    Cell(Eigen::Vector3i(x - 1, y - 1, z - 1), 1), Cell(Eigen::Vector3i(x - 1, y - 1, z), 1), Cell(Eigen::Vector3i(x - 1, y - 1, z + 1), 1),
    Cell(Eigen::Vector3i(x - 1, y    , z - 1), 1), Cell(Eigen::Vector3i(x - 1, y    , z), 1), Cell(Eigen::Vector3i(x - 1, y    , z + 1), 1),
    Cell(Eigen::Vector3i(x - 1, y + 1, z - 1), 1), Cell(Eigen::Vector3i(x - 1, y + 1, z), 1), Cell(Eigen::Vector3i(x - 1, y + 1, z + 1), 1),

    Cell(Eigen::Vector3i(x, y - 1, z - 1), 1),     Cell(Eigen::Vector3i(x, y - 1, z), 1),     Cell(Eigen::Vector3i(x, y - 1, z + 1), 1),
    Cell(Eigen::Vector3i(x, y    , z - 1), 1),                                                Cell(Eigen::Vector3i(x, y    , z + 1), 1),
    Cell(Eigen::Vector3i(x, y + 1, z - 1), 1),     Cell(Eigen::Vector3i(x, y + 1, z), 1),     Cell(Eigen::Vector3i(x, y + 1, z + 1), 1),

    Cell(Eigen::Vector3i(x + 1, y - 1, z - 1), 1), Cell(Eigen::Vector3i(x + 1, y - 1, z), 1), Cell(Eigen::Vector3i(x + 1, y - 1, z + 1), 1),
    Cell(Eigen::Vector3i(x + 1, y    , z - 1), 1), Cell(Eigen::Vector3i(x + 1, y    , z), 1), Cell(Eigen::Vector3i(x + 1, y    , z + 1), 1),
    Cell(Eigen::Vector3i(x + 1, y + 1, z - 1), 1), Cell(Eigen::Vector3i(x + 1, y + 1, z), 1), Cell(Eigen::Vector3i(x + 1, y + 1, z + 1), 1),
  };

  const std::vector<Cell> step2 = {
    Cell(Eigen::Vector3i(x - 2, y - 1, z - 1), 1), Cell(Eigen::Vector3i(x - 2, y - 1, z + 1), 1),
    Cell(Eigen::Vector3i(x - 2, y + 1, z - 1), 1), Cell(Eigen::Vector3i(x - 2, y + 1, z + 1), 1),

    Cell(Eigen::Vector3i(x - 1, y - 2, z - 1), 1), Cell(Eigen::Vector3i(x - 1, y - 2, z + 1), 1),
    Cell(Eigen::Vector3i(x - 1, y - 1, z - 2), 1), Cell(Eigen::Vector3i(x - 1, y - 1, z + 2), 1),
    Cell(Eigen::Vector3i(x - 1, y + 1, z - 2), 1), Cell(Eigen::Vector3i(x - 1, y + 1, z + 2), 1),
    Cell(Eigen::Vector3i(x - 1, y + 2, z - 1), 1), Cell(Eigen::Vector3i(x - 1, y + 2, z + 1), 1),

    Cell(Eigen::Vector3i(x + 1, y - 2, z - 1), 1), Cell(Eigen::Vector3i(x + 1, y - 2, z + 1), 1),
    Cell(Eigen::Vector3i(x + 1, y - 1, z - 2), 1), Cell(Eigen::Vector3i(x + 1, y - 1, z + 2), 1),
    Cell(Eigen::Vector3i(x + 1, y + 1, z - 2), 1), Cell(Eigen::Vector3i(x + 1, y + 1, z + 2), 1),
    Cell(Eigen::Vector3i(x + 1, y + 2, z - 1), 1), Cell(Eigen::Vector3i(x + 1, y + 2, z + 1), 1),

    Cell(Eigen::Vector3i(x + 2, y - 1, z - 1), 1), Cell(Eigen::Vector3i(x + 2, y - 1, z + 1), 1),
    Cell(Eigen::Vector3i(x + 2, y + 1, z - 1), 1), Cell(Eigen::Vector3i(x + 2, y + 1, z + 1), 1),
  };

  // Step 3 contains 66 cells, so I was too lazy to do this

  const std::vector<Cell> step4 = {
      Cell(Eigen::Vector3i(x - 3, y    , z    ), 1),
      Cell(Eigen::Vector3i(x - 2, y - 2, z - 1), 1), Cell(Eigen::Vector3i(x - 2, y - 2, z + 1), 1),
      Cell(Eigen::Vector3i(x - 2, y - 1, z - 2), 1), Cell(Eigen::Vector3i(x - 2, y - 1, z + 2), 1),
      Cell(Eigen::Vector3i(x - 2, y + 1, z - 2), 1), Cell(Eigen::Vector3i(x - 2, y + 1, z + 2), 1),
      Cell(Eigen::Vector3i(x - 2, y + 2, z - 1), 1), Cell(Eigen::Vector3i(x - 2, y + 2, z + 1), 1),

      Cell(Eigen::Vector3i(x - 1, y - 2, z - 2), 1), Cell(Eigen::Vector3i(x - 1, y - 2, z + 2), 1),
      Cell(Eigen::Vector3i(x - 1, y + 2, z - 2), 1), Cell(Eigen::Vector3i(x - 1, y + 2, z + 2), 1),

      Cell(Eigen::Vector3i(x    , y - 3, z    ), 1), Cell(Eigen::Vector3i(x    , y    , z - 3), 1),
      Cell(Eigen::Vector3i(x    , y    , z + 3), 1), Cell(Eigen::Vector3i(x    , y + 3, z    ), 1),

      Cell(Eigen::Vector3i(x + 1, y - 2, z - 2), 1), Cell(Eigen::Vector3i(x + 1, y - 2, z + 2), 1),
      Cell(Eigen::Vector3i(x + 1, y + 2, z - 2), 1), Cell(Eigen::Vector3i(x + 1, y + 2, z + 2), 1),

      Cell(Eigen::Vector3i(x + 2, y - 2, z - 1), 1), Cell(Eigen::Vector3i(x + 2, y - 2, z + 1), 1),
      Cell(Eigen::Vector3i(x + 2, y - 1, z - 2), 1), Cell(Eigen::Vector3i(x + 2, y - 1, z + 2), 1),
      Cell(Eigen::Vector3i(x + 2, y + 1, z - 2), 1), Cell(Eigen::Vector3i(x + 2, y + 1, z + 2), 1),
      Cell(Eigen::Vector3i(x + 2, y + 2, z - 1), 1), Cell(Eigen::Vector3i(x + 2, y + 2, z + 1), 1),
      Cell(Eigen::Vector3i(x + 3, y    , z    ), 1),
  };

  const int expectedStepsSize = 14;
  const std::vector<Cell>* expectedSteps[expectedStepsSize] = {
      &step0, &step1,
      &step2, nullptr, &step4,
      &step2, nullptr, &step4,
      &step2, nullptr, &step4,
  };

  // clang-format on

  auto born = boost::dynamic_bitset<>(27);
  born[4] = true;

  auto survive = boost::dynamic_bitset<>(27);
  survive[5] = true;

  WorldImplSimple world;
  InitWorld(world, {30, 30, 30}, step0);
  world.SetBorn(born);
  world.SetSurvive(survive);

  for (int i = 0; i < expectedStepsSize; ++i) {
    if (expectedSteps[i] == nullptr) {
      world.Update();
      continue;
    }

    const std::vector<Cell>& currStep = *expectedSteps[i];
    auto iter = world.MakeWholeIter();
    size_t processedCount = 0;

    while (const Cell* cell = iter.Next()) {
      processedCount += 1;
      auto res = std::find(currStep.begin(), currStep.end(), *cell);

      ASSERT_TRUE(res != currStep.end())
          << "Incorrect generation, step - " << i
          << ": Cannot find generated cell: (" << cell->pos.transpose() << "), "
          << (int)cell->state;
    }

    ASSERT_EQ(processedCount, currStep.size())
        << "Incorrect generation, step - " << i
        << ": Not all cells are generated";

    world.Update();
  }
}

TEST(WorldImplSimple, SetCell) {
  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, {});

  Cell cell = Cell(Eigen::Vector3i(1, 1, 1), 1);
  world.SetCell(cell);
  ASSERT_EQ(world.GetCell(cell.pos), cell.state);

  cell = Cell(Eigen::Vector3i(9, 9, 9), 9);
  world.SetCell(cell);
  ASSERT_EQ(world.GetCell(cell.pos), cell.state);
}

TEST(WorldImplSimple, SetCellTwice) {
  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, {});

  Cell cell = Cell(Eigen::Vector3i(1, 1, 1), 1);
  world.SetCell(cell);
  ASSERT_EQ(world.GetCell(cell.pos), cell.state);

  cell = Cell(Eigen::Vector3i(1, 1, 1), 2);
  world.SetCell(cell);
  ASSERT_EQ(world.GetCell(cell.pos), cell.state);
}

TEST(WorldImplSimple, SetInvalidCell) {
  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, {});

  Cell cell = Cell(Eigen::Vector3i(-1, -1, -1), 1);
  world.SetCell(cell);
  ASSERT_FALSE(world.GetCell(cell.pos));

  cell = Cell(Eigen::Vector3i(11, 11, 11), 11);
  world.SetCell(cell);
  ASSERT_FALSE(world.GetCell(cell.pos));
}

static void CheckAndDeleteCell(const Cell& cell, int min, int max,
                               std::vector<Cell>& expCells) {
  ASSERT_TRUE(Axis::WithinBound(min, max, cell.state))
      << "Cell is not in range: expcted [" << min << ", " << max << "] "
      << "but actual: " << cell.state;

  auto res = std::find(expCells.begin(), expCells.end(), cell);
  ASSERT_TRUE(res != expCells.end());
  expCells.erase(res);
}

TEST(WorldImplSimple, Iterator) {
  WorldImplSimple world;
  const Area area = Area(1, 10);
  auto expCells = GenerateCellList(area);

  InitWorld(world, {10, 10, 10}, expCells);
  auto iter = world.MakeWholeIter();

  while (const Cell* cell = iter.Next()) {
    CheckAndDeleteCell(*cell, area.min[0], area.max[0], expCells);
  }

  ASSERT_TRUE(expCells.empty()) << "Iterator did not go through all the values";
}

TEST(WorldImplSimple, ValidAreaIter) {
  WorldImplSimple world;
  const Area area = Area(4, 7);
  const auto cells = GenerateCellList(Area(1, 10));
  auto expCells = GenerateCellList(area);

  InitWorld(world, {10, 10, 10}, cells);
  auto iter = world.MakeAreaIter(area);

  while (const Cell* cell = iter.Next()) {
    CheckAndDeleteCell(*cell, area.min[0], area.max[0], expCells);
  }

  ASSERT_TRUE(expCells.empty()) << "Iterator did not go through all the values";
}

TEST(WorldImplSimple, OutOfRangeAreaIter) {
  WorldImplSimple world;
  const Area area = Area(-1234, 1234);
  const auto cells = GenerateCellList(Area(1, 10));
  auto expCells = cells;

  InitWorld(world, {10, 10, 10}, cells);
  auto iter = world.MakeAreaIter(area);

  while (const Cell* cell = iter.Next()) {
    CheckAndDeleteCell(*cell, area.min[0], area.max[0], expCells);
  }

  ASSERT_TRUE(expCells.empty()) << "Iterator did not go through all the values";
}

TEST(WorldImplSimple, InvalidAreaIterator) {
  WorldImplSimple world;
  const Area area = Area(4321, -128);
  const auto cells = GenerateCellList(Area(1, 10));

  InitWorld(world, {10, 10, 10}, cells);
  auto iter = world.MakeAreaIter(area);

  ASSERT_FALSE(iter.Next()) << "Iterator contain value, but area is invalid";
}

TEST(WorldImplSimple, NegativeAreaIter) {
  WorldImplSimple world;
  const Area area = Area(-256, 0);
  const auto cells = GenerateCellList(Area(1, 10));

  InitWorld(world, {10, 10, 10}, cells);
  auto iter = world.MakeAreaIter(area);

  ASSERT_FALSE(iter.Next()) << "Iterator contain value, but area is invalid";
}
