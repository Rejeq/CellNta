#include <gtest/gtest.h>

#include <algorithm>
#include <exception>
#include <random>

#include <Cellnta/Axis.h>
#include <Cellnta/World/Impl/Simple.h>

#include "Utils.h"

using namespace Cellnta;

using MaskList = std::vector<Rule::Mask>;

MaskList MakeUniformDistribMasks(const Rule& rule, int divider,
                                 int quanMin = -1, int quanMax = -1,
                                 int stateMin = -1, int stateMax = -1) {
  assert(divider > 0);

  if (quanMin == -1)
    quanMin = 1;
  if (quanMax == -1)
    quanMax = rule.GetTotalNeighbors();

  if (stateMin == -1)
    stateMin = 1;
  if (stateMax == -1)
    stateMax = rule.GetTotalStates();

  MaskList maskList;
  const float quanStep = (float)(quanMax - quanMin) / divider;

  for (Cell::State state = stateMin; state < stateMax; state++) {
    int oldQuantity = quanMin - 1;

    for (float quantIt = quanMin; quantIt <= quanMax; quantIt += quanStep) {
      const int quantity = quantIt;
      if (oldQuantity == quantity)
        continue;
      oldQuantity = quantity;

      Rule::Mask mask = rule.MakeMask(0, state, quantity);
      assert(std::find(maskList.begin(), maskList.end(), mask) ==
             maskList.end());

      maskList.emplace_back(mask);
    }
  }

  return maskList;
}

TEST(Rule, ProvideCorrectLookup) {
  Rule rule = Rule(4, 3);

  for (Cell::State state = 1; state < rule.GetTotalStates(); state++) {
    for (int quantity = 1; quantity < rule.GetTotalNeighbors(); quantity++) {
      Rule::Mask mask = rule.MakeMask(0, state, quantity);

      rule.SetState(state, mask, state - 1);
      ASSERT_EQ(rule.GetState(state, mask), state - 1);
    }
  }
}

TEST(Rule, SetState_StoreFallbackState) {
  constexpr Cell::State Target = 1;
  constexpr Cell::State Fallback = 1;

  Rule rule = Rule(4, 2);
  Rule::Mask mask = rule.MakeMaskFor(0).WhenQuantity(3);

  rule.SetFallbackState(Target, Fallback);
  rule.SetState(Target, mask, Fallback);
  rule.SetFallbackState(Target, 0);

  ASSERT_EQ(rule.GetState(Target, mask), Fallback);
}

TEST(Rule, GetState_ReturnsFallbackWhenNotFound) {
  constexpr Cell::State Target = 1;

  Rule rule = Rule(4, 2);
  rule.SetFallbackState(Target, 1);

  auto state = rule.GetState(Target, rule.MakeMaskFor(0).WhenQuantity(3));

  ASSERT_EQ(state, rule.GetFallbackState(Target));
}

TEST(Rule, MakeMask_GivesUniqueValues) {
  constexpr int Dims = 3;
  constexpr Cell::State MaxStates = 8;

  Rule rule = Rule(Dims, MaxStates);
  std::set<Rule::Mask> existed;

  existed.emplace(rule.MakeMaskFor(0).WhenQuantity(1));
  for (Cell::State state = 1; state < MaxStates; state++) {
    for (int quantity = 1; quantity <= rule.GetTotalNeighbors(); quantity++) {
      Rule::Mask newMask = rule.MakeMask(0, state, quantity);

      ASSERT_TRUE(existed.find(newMask) == existed.end());
      existed.emplace(newMask);
    }
  }
}

TEST(Rule, MakeMask_GivesSameValuesForZeroState) {
  constexpr int Dims = 3;
  constexpr Cell::State MaxStates = 8;

  Rule rule = Rule(Dims, MaxStates);
  Rule::Mask existedMask;

  existedMask = rule.MakeMaskFor(0).WhenQuantity(1);
  for (int quantity = 2; quantity < rule.GetTotalNeighbors(); quantity++) {
    Rule::Mask newMask = rule.MakeMask(0, 0, quantity);

    ASSERT_TRUE(existedMask == newMask);
  }
}

TEST(Rule, MakeMask_AccumulatesQuantitiesForSameState) {
  constexpr Cell::State State = 9;
  constexpr size_t QuanPerCall = 5;
  constexpr size_t TotalIter = 3;

  Rule rule = Rule(3, 16);
  Rule::Mask mask = 0;

  for (size_t i = 0; i < TotalIter; i++)
    mask = rule.MakeMask(mask, State, QuanPerCall);

  ASSERT_EQ(mask,
            rule.MakeMaskFor(State).WhenQuantity(QuanPerCall * TotalIter));
}

TEST(Rule, ChangeDimension_NotTouchingMasks) {
  constexpr int FromDim = 2;
  constexpr int ToDim = 3;

  Rule rule = Rule(FromDim, 8);
  Rule tmpRule = Rule(ToDim, 8);
  MaskList allMasks = MakeUniformDistribMasks(rule, 2);
  MaskList allNewMasks =
      MakeUniformDistribMasks(tmpRule, 2, -1, rule.GetTotalNeighbors());

  for (auto& mask : allMasks)
    rule.SetState(1, mask, 2);

  Rule newRule = rule.ChangeDimension(ToDim);

  assert(allMasks.size() == allNewMasks.size());
  Rule::SpecialStateIter iter = newRule.MakeSpecialStatesIter(1);
  for (size_t i = 0; i < allNewMasks.size(); i++) {
    const auto* state = iter.Next();
    ASSERT_TRUE(state != nullptr);

    auto res = std::find(allNewMasks.begin(), allNewMasks.end(), state->mask);

    ASSERT_TRUE(res != allNewMasks.end());
    ASSERT_EQ(state->become, 2);
  }

  ASSERT_TRUE(iter.Next() == nullptr)
      << "ChangeDimension() adds an unspecified masks";
}

TEST(Rule, ChangeDimension_PartiallyResetsMasks) {
  constexpr int FromDim = 3;
  constexpr int ToDim = 2;

  Rule rule = Rule(FromDim, 2);
  MaskList allMasks = MakeUniformDistribMasks(rule, 4);

  for (auto& mask : allMasks)
    rule.SetState(1, mask, 0);

  Rule newRule = rule.ChangeDimension(ToDim);

  MaskList exluded;
  Rule::SpecialStateIter iter = newRule.MakeSpecialStatesIter(1);
  for (size_t i = 0; i < allMasks.size(); i++) {
    const auto* state = iter.Next();
    if (state == nullptr) {
      return;
    }

    ASSERT_EQ(state->become, 0);
  }

  ASSERT_TRUE(false) << "ChangeDimension does not delete anything";
}

TEST(Rule, ChangeDimension_FullyResetMasks) {
  constexpr int FromDim = 3;
  constexpr int ToDim = 2;

  Rule rule = Rule(FromDim, 4);
  int toDimEnd = 1 + Rule::GetTotalNeighbors(ToDim, rule.GetNeighborhood());
  MaskList allMasks = MakeUniformDistribMasks(rule, 4, toDimEnd, -1);

  for (auto& mask : allMasks)
    rule.SetState(1, mask, 2);

  Rule newRule = rule.ChangeDimension(ToDim);

  Rule::SpecialStateIter iter = newRule.MakeSpecialStatesIter(1);
  ASSERT_TRUE(iter.Next() == nullptr) << "Rule must do not have any values";
}

TEST(Rule, ChangeTotalStates_NotTouchingMasks) {
  constexpr Cell::State FromStates = 3;
  constexpr Cell::State ToStates = 8;

  Rule rule = Rule(2, FromStates);
  Rule tmpRule = Rule(2, ToStates);
  MaskList allMasks = MakeUniformDistribMasks(rule, 2);
  MaskList allNewMasks =
      MakeUniformDistribMasks(tmpRule, 2, -1, -1, -1, FromStates);

  for (auto& mask : allMasks)
    rule.SetState(1, mask, 2);

  Rule newRule = rule.ChangeTotalStates(ToStates);

  assert(allMasks.size() == allNewMasks.size());
  Rule::SpecialStateIter iter = newRule.MakeSpecialStatesIter(1);
  for (size_t i = 0; i < allMasks.size(); i++) {
    const auto* state = iter.Next();
    ASSERT_TRUE(state != nullptr);

    auto res = std::find(allNewMasks.begin(), allNewMasks.end(), state->mask);

    ASSERT_TRUE(res != allNewMasks.end());
    ASSERT_EQ(state->become, 2);
  }

  ASSERT_TRUE(iter.Next() == nullptr)
      << "ChangeDimension() adds an unspecified masks";
}

TEST(Rule, ChangeTotalStates_FullyResetMasks) {
  constexpr int FromStates = 9;
  constexpr int ToStates = 4;

  Rule rule = Rule(3, FromStates);
  MaskList allMasks =
      MakeUniformDistribMasks(rule, 4, -1, -1, ToStates + 1, -1);

  for (auto& mask : allMasks)
    rule.SetState(1, mask, 2);

  Rule newRule = rule.ChangeTotalStates(ToStates);

  Rule::SpecialStateIter iter = newRule.MakeSpecialStatesIter(1);
  ASSERT_TRUE(iter.Next() == nullptr) << "Rule must do not have any values";
}

TEST(Rule, MaskToQuantityList_SlicesByDigit) {
  constexpr Cell::State States = 10;

  Rule rule = Rule(3, States);
  Rule::Mask mask = 0;
  // Zero state is not included so get lets rid of it
  std::vector<size_t> expected(States - 1, 0);

  for (Cell::State state = 1; state < States; state += 2) {
    mask = rule.MakeMask(mask, state, state + 1);
    expected[state - 1] = state + 1;
  }

  auto list = rule.MaskToQuantityList(mask);

  ASSERT_EQ(list, expected);
}

TEST(Rule, MaskToQuantityList_DoesNotEraseZerosBeforDigit) {
  Rule rule = Rule(3, 5);
  Rule::Mask mask = rule.MakeMaskFor(1).WhenQuantity(4);
  std::vector<size_t> expected = {4, 0, 0, 0};

  auto list = rule.MaskToQuantityList(mask);

  ASSERT_EQ(list, expected);
}

TEST(Rule, MaskToQuantityList_DoesNotEraseZerosAfterDigit) {
  Rule rule = Rule(3, 5);
  Rule::Mask mask = rule.MakeMaskFor(4).WhenQuantity(3);
  std::vector<size_t> expected = {0, 0, 0, 3};

  auto list = rule.MaskToQuantityList(mask);

  ASSERT_EQ(list, expected);
}
