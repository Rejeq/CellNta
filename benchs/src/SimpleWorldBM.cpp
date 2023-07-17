#include <benchmark/benchmark.h>

#include <Cellnta/Snapshot.h>
#include <Cellnta/World/WorldList.h>

using namespace Cellnta;

const World::AxisSizeList WorldSize = {30, 30, 30};
constexpr size_t Dimensions = 3;
constexpr size_t TotalStates = 2;

static Snapshot CreateBlinker() {
  int x = 15, y = 15, z = 15;

  Snapshot blinker = {
      {Eigen::Vector3i(x, y, z), 1},     {Eigen::Vector3i(x - 1, y, z), 1},
      {Eigen::Vector3i(x + 1, y, z), 1}, {Eigen::Vector3i(x, y - 1, z), 1},
      {Eigen::Vector3i(x, y + 1, z), 1}, {Eigen::Vector3i(x, y, z - 1), 1},
      {Eigen::Vector3i(x, y, z + 1), 1},
  };

  return blinker;
}

static void BM_SimpleWorld_Blinker(benchmark::State& state) {
  auto world = CreateWorldInstance(WorldType::SIMPLE);
  if (world == nullptr)
    return;

  Rule rule(Dimensions, TotalStates);

  rule.SetState(0, rule.MakeMaskFor(Cell::State(1)).WhenQuantity(4), 1);
  rule.SetState(0, rule.MakeMaskFor(Cell::State(1)).WhenQuantity(5), 1);
  rule.SetState(1, rule.MakeMaskFor(Cell::State(1)).WhenQuantity(5), 1);

  world->SetRule(rule);
  world->SetAxisSizeList(WorldSize);

  Snapshot blinker = CreateBlinker();
  world->SetCell(blinker.MakeWholeIter());

  for (auto _ : state)
    world->Update();
}
BENCHMARK(BM_SimpleWorld_Blinker);

static void BM_SimpleWorld_Blinker_Growing(benchmark::State& state) {
  auto world = CreateWorldInstance(WorldType::SIMPLE);
  if (world == nullptr)
    return;

  Rule rule(Dimensions, TotalStates);

  rule.SetState(0, rule.MakeMaskFor(Cell::State(1)).WhenQuantity(1), 1);
  rule.SetState(0, rule.MakeMaskFor(Cell::State(1)).WhenQuantity(2), 1);
  rule.SetState(0, rule.MakeMaskFor(Cell::State(1)).WhenQuantity(3), 1);
  rule.SetState(0, rule.MakeMaskFor(Cell::State(1)).WhenQuantity(4), 1);
  rule.SetState(0, rule.MakeMaskFor(Cell::State(1)).WhenQuantity(5), 1);
  rule.SetState(1, rule.MakeMaskFor(Cell::State(1)).WhenQuantity(5), 1);

  world->SetRule(rule);
  world->SetAxisSizeList(WorldSize);

  Snapshot blinker = CreateBlinker();
  world->SetCell(blinker.MakeWholeIter());

  for (auto _ : state)
    world->Update();
}
BENCHMARK(BM_SimpleWorld_Blinker_Growing);
