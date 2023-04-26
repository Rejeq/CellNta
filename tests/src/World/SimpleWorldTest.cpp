#include <gtest/gtest.h>

#include <Cellnta/World/Impl/Simple.h>

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
  const std::vector<Cell> expectedCells = {
      // clang-format off
      Cell(Eigen::Vector3i(-12, -12, -12), -12),
      Cell(Eigen::Vector3i(0, 0, 0), 0), Cell(Eigen::Vector3i(1, 1, 1), 1),
      Cell(Eigen::Vector3i(2, 2, 2), 2), Cell(Eigen::Vector3i(3, 3, 3), 3),
      Cell(Eigen::Vector3i(4, 4, 4), 4), Cell(Eigen::Vector3i(5, 5, 5), 5),
      Cell(Eigen::Vector3i(6, 6, 6), 6), Cell(Eigen::Vector3i(7, 7, 7), 7),
      Cell(Eigen::Vector3i(8, 8, 8), 8), Cell(Eigen::Vector3i(9, 9, 9), 9),
      Cell(Eigen::Vector3i(10, 10, 10), 10), Cell(Eigen::Vector3i(150, 150, 150), 150),
      // clang-format on
  };
  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, expectedCells);

  auto iter = expectedCells.begin();

  ASSERT_FALSE(world.GetCell(iter->pos));
  ++iter;
  for (; iter != expectedCells.end() - 2; iter++) {
    ASSERT_EQ(world.GetCell(iter->pos), iter->state);
  }
  ASSERT_FALSE(world.GetCell(iter->pos));
  ++iter;
  ASSERT_FALSE(world.GetCell(iter->pos));
}

TEST(WorldImplSimple, Iterator) {
  const std::vector<Cell> expectedCells = {
      Cell(Eigen::Vector3i(1, 1, 1), 1), Cell(Eigen::Vector3i(2, 2, 2), 2),
      Cell(Eigen::Vector3i(3, 3, 3), 3), Cell(Eigen::Vector3i(4, 4, 4), 4),
      Cell(Eigen::Vector3i(5, 5, 5), 5), Cell(Eigen::Vector3i(6, 6, 6), 6),
      Cell(Eigen::Vector3i(7, 7, 7), 7), Cell(Eigen::Vector3i(8, 8, 8), 8),
      Cell(Eigen::Vector3i(9, 9, 9), 9), Cell(Eigen::Vector3i(10, 10, 10), 10),
      // Keep in mind that cell with state=10 is not iterable because it has
      // invalid position
  };
  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, expectedCells);

  auto iter = world.MakeWholeIter();
  const Cell* cell = nullptr;
  int i = 0;

  while ((cell = iter.Next()) != nullptr) {
    ASSERT_EQ(*cell, expectedCells[i]);
    ++i;
  }
  if (i == 0) {
    ASSERT_TRUE(false) << "Iterator no have any value";
  }
}

TEST(WorldImplSimple, AreaIterator) {
  const std::vector<Cell> expectedCells = {
      Cell(Eigen::Vector3i(1, 1, 1), 1), Cell(Eigen::Vector3i(2, 2, 2), 2),
      Cell(Eigen::Vector3i(3, 3, 3), 3), Cell(Eigen::Vector3i(4, 4, 4), 4),
      Cell(Eigen::Vector3i(5, 5, 5), 5), Cell(Eigen::Vector3i(6, 6, 6), 6),
      Cell(Eigen::Vector3i(7, 7, 7), 7), Cell(Eigen::Vector3i(8, 8, 8), 8),
      Cell(Eigen::Vector3i(9, 9, 9), 9), Cell(Eigen::Vector3i(10, 10, 10), 10),
      // Keep in mind that cell with state=10 is not iterable because it has
      // invalid position
  };
  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, expectedCells);

  auto iter = world.MakeAreaIter(Area(4, 7));
  const Cell* cell = nullptr;
  int i = 3;

  while ((cell = iter.Next()) != nullptr) {
    ASSERT_EQ(*cell, expectedCells[i]);
    ++i;
  }
  if (i != 6) {
    ASSERT_TRUE(false) << "Iterator did not go through all the values";
  }

  // Testing out of range area
  iter = world.MakeAreaIter(Area(-1234, 1234));
  cell = nullptr;
  i = 0;

  while ((cell = iter.Next()) != nullptr) {
    ASSERT_EQ(*cell, expectedCells[i]);
    ++i;
  }
  if (i != 9) {
    ASSERT_TRUE(false) << "Iterator did not go through all the values";
  }

  // Testing invalid area
  iter = world.MakeAreaIter(Area(4321, -128));
  ASSERT_FALSE(iter.Next()) << "Iterator contain value, but area is invalid";
}
