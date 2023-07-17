#include "Cellnta/World/Rule.h"

#include "Cellnta/Log.h"

using namespace Cellnta;

Rule::Rule(int dimension, Cell::State totalStates,
           Rule::Neighborhood neighborhood)
    : m_neighborhood(neighborhood), m_dim(dimension) {
  assert(dimension > 0 && "Dimension must be larger than 0");
  assert(totalStates > 0 && "Total states must be larger than 0");

  m_data.resize((size_t)totalStates);

  m_data[0].maskOffset = 0;
  for (size_t i = 1; i < m_data.size(); i++)
    m_data[i].maskOffset = std::pow(GetMaskBase(), i - 1);
}

bool Rule::SetState(Cell::State oldState, Rule::Mask mask,
                    Cell::State newState) {
  assert((IsValidState(oldState) && IsValidState(newState)) &&
         "Cell::State must be valid in Rule");

  StateData& stateData = m_data[oldState];
  stateData.special.insert_or_assign(mask, newState);
  return false;
}

Cell::State Rule::GetState(Cell::State targetState, Rule::Mask mask) const {
  assert(IsValidState(targetState) && "Cell::State must be valid in Rule");

  const StateData& stateData = m_data[targetState];

  const auto& res = stateData.special.find(mask);
  if (res == stateData.special.end())
    return stateData.fallback;

  return res->second;
}

bool Rule::ResetState(Cell::State state, Rule::Mask mask) {
  assert(IsValidState(state) && "Cell::State must be valid in Rule");

  StateData& stateData = m_data[state];

  auto it = stateData.special.find(mask);
  if (it == stateData.special.end())
    return true;

  stateData.special.erase(it);
  return false;
}

bool Rule::ReplaceState(Cell::State state, Rule::Mask oldMask,
                        Rule::Mask newMask) {
  assert(IsValidState(state) && "Cell::State must be valid in Rule");

  StateData& stateData = m_data[state];

  auto node = stateData.special.extract(oldMask);
  if (node.empty())
    return true;

  node.key() = newMask;

  stateData.special.insert(std::move(node));
  return false;
}

bool Rule::ReplaceState(Cell::State state, Mask oldMask, Mask newMask,
                        Cell::State newBecome) {
  assert(IsValidState(state) && IsValidState(newBecome) &&
         "Cell::State must be valid in Rule");

  StateData& stateData = m_data[state];

  auto node = stateData.special.extract(oldMask);
  if (node.empty())
    return true;

  node.key() = newMask;
  node.mapped() = newBecome;

  stateData.special.insert(std::move(node));
  return false;
}

void Rule::SetFallbackState(Cell::State oldState, Cell::State newState) {
  assert((IsValidState(oldState) && IsValidState(newState)) &&
         "Cell::State must be valid in Rule");

  StateData& stateData = m_data[oldState];
  stateData.fallback = newState;
}

Cell::State Rule::GetFallbackState(Cell::State state) const {
  assert(IsValidState(state) && "Cell::State must be valid in Rule");

  return m_data[state].fallback;
}

Rule::MaskBuilder::MaskBuilder(const Rule& rule, Cell::State state)
    : m_rule(&rule), m_state(state) {}

Rule::MaskBuilder& Rule::MaskBuilder::WhenQuantity(int quantity) {
  m_quantity = quantity;
  return *this;
}

Rule::MaskBuilder& Rule::MaskBuilder::From(Rule::Mask from) {
  m_from = from;
  return *this;
}

Rule::MaskBuilder::operator Rule::Mask() const {
  assert(m_quantity != -1 && "Quantity must be specified");

  return m_rule->MakeMask(m_from, m_state, m_quantity);
}

Rule::MaskBuilder Rule::MakeMaskFor(Cell::State state) const {
  return MaskBuilder(*this, state);
}

Rule::Mask Rule::MakeMask(Rule::Mask oldMask, Cell::State newState) const {
  assert(IsValidState(newState) && "Cell::State must be valid in Rule");

  return oldMask + m_data[newState].maskOffset;
}

Rule::Mask Rule::MakeMask(Rule::Mask oldMask, Cell::State newState,
                          int count) const {
  assert(IsValidState(newState) && "Cell::State must be valid in Rule");
  assert(count > 0 && count <= GetTotalNeighbors() &&
         "Count must be in range (0. totalNeighbors] in Rule");

  const Mask newVal = m_data[newState].maskOffset * count;
  return oldMask + newVal;
}

Rule Rule::ChangeDimension(int newDim) const {
  const Rule& oldRule = *this;
  Rule newRule(newDim, oldRule.GetTotalStates(), oldRule.GetNeighborhood());

  const int oldBase = oldRule.GetMaskBase();
  const int newBase = newRule.GetMaskBase();

  for (size_t i = 0; i < newRule.m_data.size(); i++) {
    StateData& stateData = newRule.m_data[i];

    for (const auto& oldMaskNode : oldRule.m_data[i].special) {
      Mask oldMask = oldMaskNode.first;
      Mask newMask = 0;
      Cell::State state = oldMaskNode.second;

      Cell::State digitIdx = 1;
      while (oldMask != 0) {
        assert(digitIdx < newRule.GetTotalStates() && "Mask is broken");

        int digit = oldMask % oldBase;

        // Check for reseting current digit
        if (digit < newBase && digit != 0) {
          newMask += digit * newRule.m_data[digitIdx].maskOffset;
        }

        oldMask /= oldBase;
        digitIdx++;
      }

      if (newMask != 0)
        stateData.special.insert({newMask, state});
    }
  }

  return newRule;
}

int Rule::GetTotalNeighbors(int dim, Neighborhood neighborhood) {
  switch (neighborhood) {
    case Neighborhood::Moore: return std::pow(3, dim) - 1;
    case Neighborhood::VonNeumann: return dim * 2;
    default: assert("Unreachable");
  }

  return -1;
}

int Rule::GetTotalNeighbors() const {
  return GetTotalNeighbors(GetDimension(), m_neighborhood);
}

Rule Rule::ChangeTotalStates(int newTotalStates) const {
  const Rule& oldRule = *this;
  Rule newRule(oldRule.GetDimension(), newTotalStates,
               oldRule.GetNeighborhood());

  CopyMasks(oldRule, newRule);
  return newRule;
}

Rule Rule::ChangeNeighborhood(Neighborhood newNeighborhood) const {
  const Rule& oldRule = *this;
  Rule newRule(oldRule.GetDimension(), oldRule.GetTotalStates(),
               newNeighborhood);

  CopyMasks(oldRule, newRule);
  return newRule;
}

std::vector<size_t> Rule::MaskToQuantityList(Rule::Mask mask) const {
  std::vector<size_t> quanList(GetTotalStates() - 1, 0);

  auto it = quanList.begin();
  while (mask != 0) {
    *it = mask % GetMaskBase();
    mask /= GetMaskBase();

    it++;
  }

  return quanList;
}

Rule::Mask Rule::QuantityListToMask(const std::vector<size_t>& list) const {
  assert(list.size() == GetTotalStates() - 1);

  Mask mask = 0;
  for (Cell::State state = 1; state < list.size() + 1; state++) {
    mask = MakeMask(mask, state, list[state - 1]);
  }

  return mask;
}

Rule::SpecialStateIter Rule::MakeSpecialStatesIter(Cell::State state) const {
  assert(IsValidState(state) && "Cell::State must be valid in Rule");
  const auto& spec = m_data[state].special;
  return SpecialStateIter(spec.begin(), spec.end());
}

void Rule::SpecialStateIter::Reset() {
  m_curr = m_begin;
}

const Rule::SpecialStateData* Rule::SpecialStateIter::Next() {
  if (m_curr == m_end)
    return nullptr;

  m_currSpecData.mask = m_curr->first;
  m_currSpecData.become = m_curr->second;
  m_curr++;

  return &m_currSpecData;
}

void Rule::CopyMasks(const Rule& from, Rule& to) const {
  const size_t minStates = std::min(from.m_data.size(), to.m_data.size());

  for (size_t i = 0; i < minStates; i++) {
    StateData& stateData = to.m_data[i];

    for (const auto& fromMaskNode : from.m_data[i].special) {
      Mask fromMask = fromMaskNode.first;
      Cell::State state = fromMaskNode.second;

      if (fromMask < to.GetMaxMask())
        stateData.special.insert({fromMask, state});
    }
  }
}

inline Rule::Mask Rule::GetMaxMask() {
  return std::pow(GetMaskBase(), GetTotalStates());
}

inline bool Rule::IsValidState(Cell::State state) const {
  return (state >= 0 && state < GetTotalStates());
}
