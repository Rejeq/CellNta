#pragma once

#include <unordered_map>
#include <vector>

#include <Cellnta/Cell.h>
#include <Cellnta/Iterator.h>

namespace Cellnta {

class Rule {
 public:
  enum class Neighborhood {
    // TODO: None - Any relative cell
    Moore,       // All neighbors ((3 ^ dim) - 1)
    VonNeumann,  // Orthogonal neighbors only (dim * 2)
    // TODO: Hexagonal - Moore without first and last (dim ^ 3 - 3)
  };

  // Stores neighbor info around current cell
  //
  // For example:
  // If rule have 4 states and 2 dimension with Moore neighborhood
  // Totally we have a 3^2 - 1 = 8 neighbors
  //
  // Every state can store maximum 8 neighbors
  // So we can store this values in 9-digit system (+1, since 0 neighbors also
  // needs)
  // Every digit represent count of neighbors that it has 0600 - means
  // that cell contain 6 neighbors with Cell::State(3)
  //
  // TODO: Instead of uint64_t use unlimited integer or 256 bytes
  // Or change algorithm, since its not friendly for big count of states
  using Mask = uint64_t;

  class MaskBuilder {
   public:
    MaskBuilder(const Rule& rule, Cell::State state);
    MaskBuilder& WhenQuantity(int quantity);
    MaskBuilder& From(Mask from);
    operator Mask() const;

   private:
    const Rule* m_rule;
    Cell::State m_state;
    Mask m_from = 0;
    int m_quantity = -1;
  };

  using SpecialStateContainer = std::unordered_map<Mask, Cell::State>;
  class SpecialStateIter;
  struct SpecialStateData;

  Rule(int dimension, Cell::State totalStates,
       Neighborhood neighborhood = Neighborhood::Moore);

  // Read as: oldState become newState when mask contain equal quantity of
  // states around current cell
  bool SetState(Cell::State oldState, Mask mask, Cell::State newState);
  Cell::State GetState(Cell::State targetState, Mask mask) const;

  // Resets mask for state
  // Its not same as SetState(state, mask, GetFallbackState(state))
  // Since SetState(...) will be broken when fallback for current state has been
  // changed
  //
  // Returns true when mask is not found
  // Otherwise returns false
  bool ResetState(Cell::State state, Mask mask);

  // Replace old mask with new
  //
  // Returns true when oldMask is not found
  // Otherwise returns false
  bool ReplaceState(Cell::State state, Mask oldMask, Mask newMask);
  bool ReplaceState(Cell::State state, Mask oldMask, Mask newMask,
                    Cell::State newBecome);

  // newState will be applied when neighbor mask is not found, by default its 0
  void SetFallbackState(Cell::State oldState, Cell::State newState);
  Cell::State GetFallbackState(Cell::State state) const;

  MaskBuilder MakeMaskFor(Cell::State state) const;
  Mask MakeMask(Mask oldMask, Cell::State newState) const;
  Mask MakeMask(Mask oldMask, Cell::State newState, int count) const;

  // Creates same rule with new dimension.
  // If rule has invalid Mask then digit that represent invalid state will be
  // reseted
  // Converting to a higher dimension is safe
  //
  // For example:
  // If rule with 3 dimensions, 4 states and Moore neighborhood
  // has mask - [0, 12, 7, 8]
  // And we want to convert this rule to 2 dimensions
  // Then some digits in mask will be destroyed,
  // Since in 2d we have maximum 8 neighbors
  // new mask - [0, 0, 7, 8]
  Rule ChangeDimension(int newDim) const;
  int GetDimension() const { return m_dim; }

  static int GetTotalNeighbors(int dim, Neighborhood neighborhood);
  int GetTotalNeighbors() const;
  int GetMaskBase() const { return GetTotalNeighbors() + 1; }

  // Creates same rule with new total states
  // Masks will be truncated if converting to lower totalStates
  // Converting to a higher totalStates is safe
  Rule ChangeTotalStates(int newTotalStates) const;
  Cell::State GetTotalStates() const { return m_data.size(); }

  // Creates same rule with new total states
  // If rule has invalid Mask then digit that represent invalid state will be
  // reseted
  Rule ChangeNeighborhood(Neighborhood newNeighborhood) const;
  Neighborhood GetNeighborhood() const { return m_neighborhood; }

  std::vector<size_t> MaskToQuantityList(Mask mask) const;
  Mask QuantityListToMask(const std::vector<size_t>& list) const;

  SpecialStateIter MakeSpecialStatesIter(Cell::State state) const;

  struct SpecialStateData {
    Mask mask;
    Cell::State become;
  };

  class SpecialStateIter : public IterBase::CellForwardEx<SpecialStateData> {
   public:
    SpecialStateIter(SpecialStateContainer::const_iterator begin,
                     SpecialStateContainer::const_iterator end)
        : m_begin(begin), m_end(end) {
      Reset();
    }

    void Reset() override;
    const SpecialStateData* Next() override;

   private:
    SpecialStateContainer::const_iterator m_begin;
    SpecialStateContainer::const_iterator m_end;
    SpecialStateContainer::const_iterator m_curr;
    SpecialStateData m_currSpecData;
  };

 private:
  void CopyMasks(const Rule& from, Rule& to) const;
  Mask GetMaxMask();
  bool IsValidState(Cell::State state) const;

  struct StateData {
    SpecialStateContainer special;
    Cell::State fallback = 0;
    size_t maskOffset = 0;
  };

  std::vector<StateData> m_data;

  Neighborhood m_neighborhood;
  int m_dim;
};

}  // namespace Cellnta
