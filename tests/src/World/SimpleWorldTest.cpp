#include <gtest/gtest.h>

#include <Cellnta/Axis.h>
#include <Cellnta/World/Impl/Simple.h>

#include "Utils.h"

using namespace Cellnta;

static void InitWorld(WorldImplSimple& world,
                      const World::AxisSizeList& axisSize,
                      const std::vector<Cell> expected) {
  world.SetRule(
      Cellnta::Rule(axisSize.size(), 2, Cellnta::Rule::Neighborhood::Moore));
  world.SetAxisSizeList(axisSize);

  ASSERT_EQ(world.GetDimension(), axisSize.size());
  ASSERT_EQ(world.GetAxisSizeList(), axisSize);

  for (const auto& cell : expected)
    world.SetCell(cell);
}

TEST(WorldImplSimple, Update_BlinkerCorrectness) {
  int x = 5, y = 5, z = 5;

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

  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, step0);
  Rule rule = world.GetRule();

  rule.SetFallbackState(0, 0);
  rule.SetFallbackState(1, 0);

  rule.SetState(0, rule.MakeMaskFor(Cell::State(1)).WhenQuantity(4), 1);
  rule.SetState(0, rule.MakeMaskFor(Cell::State(1)).WhenQuantity(5), 1);
  rule.SetState(1, rule.MakeMaskFor(Cell::State(1)).WhenQuantity(5), 1);

  world.SetRule(rule);

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

TEST(WorldImplSimple, SetCell_CanBeReaded) {
  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, {});

  Cell cell = Cell(Eigen::Vector3i(1, 1, 1), 1);
  world.SetCell(cell);
  ASSERT_EQ(world.GetCell(cell.pos), cell.state);

  cell = Cell(Eigen::Vector3i(9, 9, 9), 9);
  world.SetCell(cell);
  ASSERT_EQ(world.GetCell(cell.pos), cell.state);
}

TEST(WorldImplSimple, SetCell_OverridesPreviousCell) {
  const Cell::Pos Pos = Eigen::Vector3i(1, 1, 1);

  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, {});

  world.SetCell({Pos, 1});
  world.SetCell({Pos, 2});
  Cell::State state = world.GetCell(Pos);

  ASSERT_EQ(state, 2);
}

TEST(WorldImplSimple, SetCell_IncorrectCellCannotBeReaded) {
  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, {});

  Cell cell = Cell(Eigen::Vector3i(-1, -1, -1), 1);
  world.SetCell(cell);
  ASSERT_FALSE(world.GetCell(cell.pos));

  cell = Cell(Eigen::Vector3i(11, 11, 11), 1);
  world.SetCell(cell);
  ASSERT_FALSE(world.GetCell(cell.pos));
}

TEST(WorldImplSimple, SetCell_IncorrectStateMustBeReaded) {
  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, {});

  // World has only 2 total states
  Cell cell = Cell(Eigen::Vector3i(5, 5, 5), 7);
  world.SetCell(cell);

  ASSERT_EQ(world.GetCell(cell.pos), 7);
}

#define CHECK_AND_DELETE_CELL(cell, expCells)                           \
  do {                                                                  \
    auto res = std::find((expCells).begin(), (expCells).end(), (cell)); \
    ASSERT_TRUE(res != (expCells).end());                               \
                                                                        \
    /* Erase is requred here because cell should only occur once */     \
    (expCells).erase(res);                                              \
  } while (false)

TEST(WorldImplSimple, MakeWholeIter_BypassesAllCells) {
  const Area GenerateArea = Area(1, 10);

  auto expCells = GenerateCellList(GenerateArea);

  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, expCells);

  auto iter = world.MakeWholeIter();

  while (const Cell* cell = iter.Next()) {
    CHECK_AND_DELETE_CELL(*cell, expCells);
  }

  ASSERT_TRUE(expCells.empty()) << "Iterator did not go through all the values";
}

TEST(WorldImplSimple, MakeAreaIter_BypassesAllCellsWithinArea) {
  const Area BypassArea = Area(4, 7);
  const Area WorldArea = Area(1, 10);

  const auto cells = GenerateCellList(WorldArea);
  auto expCells = GenerateCellList(BypassArea);

  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, cells);

  auto iter = world.MakeAreaIter(BypassArea);

  while (const Cell* cell = iter.Next()) {
    CHECK_AND_DELETE_CELL(*cell, expCells);
  }

  ASSERT_TRUE(expCells.empty()) << "Iterator did not go through all the values";
}

TEST(WorldImplSimple, MakeAreaIter_BypassesAllCellsWithinDifferentAxisArea) {
  const Area BypassArea =
      Area(Eigen::Vector3i(6, -32, -32), Eigen::Vector3i(12, 32, 32));

  std::vector<Cell> cells = {
      {Eigen::Vector3i(6, 6, 6), 1},
      {Eigen::Vector3i(6, 7, 7), 1},
      {Eigen::Vector3i(7, 7, 7), 1},
      {Eigen::Vector3i(6, 0, 1), 1},
  };

  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, cells);

  auto iter = world.MakeAreaIter(BypassArea);

  while (const Cell* cell = iter.Next()) {
    CHECK_AND_DELETE_CELL(*cell, cells);
  }

  ASSERT_TRUE(cells.empty()) << "Iterator did not go through all the values";
}

TEST(WorldImplSimple, MakeAreaIter_TooLargeAreaBypassesAllCellsWithinWorld) {
  const Area LargeArea = Area(-1234, 1234);
  const Area WorldArea = Area(1, 10);

  const auto cells = GenerateCellList(WorldArea);
  auto expCells = cells;

  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, cells);

  auto iter = world.MakeAreaIter(LargeArea);

  while (const Cell* cell = iter.Next()) {
    CHECK_AND_DELETE_CELL(*cell, expCells);
  }

  ASSERT_TRUE(expCells.empty()) << "Iterator did not go through all the values";
}

TEST(WorldImplSimple, MakeAreaIter_InvalidAreaMustNotContainAnyCells) {
  const Area InvalidArea = Area(4321, -128);
  const Area WorldArea = Area(1, 10);

  const auto cells = GenerateCellList(WorldArea);

  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, cells);

  auto iter = world.MakeAreaIter(InvalidArea);

  ASSERT_FALSE(iter.Next()) << "Iterator contain value, but area is invalid";
}

TEST(WorldImplSimple, MakeAreaIter_NegativeAreaMustNotContainAnyCells) {
  const Area NegativeArea = Area(-256, 0);
  const Area WorldArea = Area(1, 10);

  const auto cells = GenerateCellList(WorldArea);

  WorldImplSimple world;
  InitWorld(world, {10, 10, 10}, cells);

  auto iter = world.MakeAreaIter(NegativeArea);

  ASSERT_FALSE(iter.Next()) << "Iterator contain value, but area is invalid";
}
