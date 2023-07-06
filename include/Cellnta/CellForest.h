#pragma once

#include <map>
#include <stack>
#include <type_traits>

#include "Cellnta/Area.h"
#include "Cellnta/Axis.h"
#include "Cellnta/Cell.h"
#include "Cellnta/Config.h"
#include "Cellnta/Iterator.h"
#include "Cellnta/Log.h"

// TODO: Don't forget rewrite without templates, when NCellStorage will be
// splitted.

namespace Cellnta {

/*
Inputs:
Vec(15, 16, 20, 30): 1
Vec(15, 20): 2
Vec(15, 20, 30): 3
Vec(1, 1, 1, 1): 4
Vec(1, 2, 5): 5
Vec(1, 3): 6

Result:
 15            1
 /\          / | \
16 20: 2    1  2  3: 6
|   |       |  |
20 30: 3    1  5: 5
|           |
30: 1       1: 4
**/
template <typename Cell = Cellnta::Cell>
class CellForest {
  struct Node;
  class WholeIter;
  class AreaIter;

  using CellState = typename Cell::State;
  using Pos = typename Cell::Pos;
  using Scalar = typename Cell::Pos::Scalar;

  using Map = std::map<Scalar, Node, std::less<Scalar>>;
  using MapConstIter = typename Map::const_iterator;
  using MapIter = typename Map::iterator;

 public:
  void Set(const Pos& pos, CellState state) {
    CELLNTA_PROFILE;

    if (IsPosInvalid(pos))
      return;

    Node& node = GetChildNode(pos);
    SetNodeState(node, state);
  }

  CellState Find(const Pos& pos) const {
    CELLNTA_PROFILE;

    if (IsPosInvalid(pos))
      return 0;

    const Map* parent = &m_roots;
    auto iter = FindChildIter(parent, pos);
    if (iter == parent->end())
      return 0;

    return iter->second.state;
  }

  CellState Erase(const Pos& pos) {
    CELLNTA_PROFILE;

    if (IsPosInvalid(pos))
      return 0;

    Map* parent = &m_roots;
    auto iter = FindChildIter(parent, pos);
    if (iter == parent->end()) {
      CELLNTA_LOG_TRACE(
          "Unable to erase position from CellForest: Cannot find node");
      return 0;
    }

    CellState state = iter->second.state;
    DeleteNode(*parent, iter);
    return state;
  }

  void EraseArea(const Area& area) {
    CELLNTA_PROFILE;

    if (IsAreaInvalid(area))
      return;

    std::stack<std::pair<Map*, IterRange<MapIter>>> stack;

    const auto PushToStack = [&area, &stack](Map& cont, int idx) {
      if (cont.empty())
        return;

      auto minRoot = cont.lower_bound(Axis::NotLess(area.MinAxis(idx)));
      if (minRoot == cont.end()) {
        if (Axis::NotLess(area.MinAxis(idx)) < cont.begin()->first)
          minRoot = cont.begin();
      }

      auto maxRoot = cont.upper_bound(Axis::GreaterThan(area.MaxAxis(idx)));
      if (maxRoot == cont.begin() && minRoot != cont.begin()) {
        if (Axis::GreaterThan(area.MaxAxis(idx)) < cont.begin()->first)
          maxRoot = cont.end();
      }

      stack.push(std::make_pair(
          &cont, IterRange<MapIter>(std::move(minRoot), std::move(maxRoot))));
    };

    PushToStack(m_roots, 0);
    while (!stack.empty()) {
      auto& currStack = stack.top();
      IterRange<MapIter>& curr = currStack.second;

      if (curr.it == curr.end) {
        stack.pop();
        continue;
      }

      Map& currCont = *currStack.first;
      Node& currNode = curr.it->second;

      if (!currNode.child.empty()) {
        PushToStack(currNode.child, stack.size());
        SetNodeState(currNode, 0);
        ++curr.it;
        continue;
      }

      curr.it = DeleteNode(currCont, curr.it);
    }
  }

  // Move state from one position to another
  // If *from* does not exist, then *to* replaced by 0
  void Move(const Pos& from, const Pos& to) {
    CELLNTA_PROFILE;

    if (IsPosInvalid(from) || IsPosInvalid(to)) {
      CELLNTA_LOG_TRACE(
          "Unable to move positions in CellForest: Passing a invalid pos");
      return;
    }

    CellState state = Erase(from);
    Set(to, state);
  }

  // Swap state beetween *first* and *second* positions
  // If one of the positions does not exist, its state will be 0
  void Swap(const Pos& first, const Pos& second) {
    CELLNTA_PROFILE;

    if (IsPosInvalid(first) || IsPosInvalid(second)) {
      CELLNTA_LOG_TRACE(
          "Unable to swap positions in CellForest: Passing a invalid pos");
      return;
    }

    auto firstNode = GetChildNode(first);
    auto secNode = GetChildNode(second);

    std::swap(firstNode.state, secNode.state);
  }

  void Clear() {
    CELLNTA_PROFILE;

    m_roots.clear();
    m_size = 0;
    m_capacity = 0;
  }

  // TODO: Add method Shrink() that erase branches that do not contain an active
  // state

  // Return count of active cells (i.e state != 0)
  int Size() const { return m_size; }

  // Return count of all allocated cells
  int Capacity() const { return m_capacity; }

  bool Empty() const { return Size() == 0; }

  WholeIter MakeWholeIter() const { return WholeIter(*this); }
  AreaIter MakeAreaIter(const Area& area) const {
    return AreaIter(*this, area);
  }

 private:
  bool IsPosInvalid(const Pos& pos) const {
    CELLNTA_PROFILE;

    if (pos.size() <= 0) {
      CELLNTA_LOG_ERROR(
          "Unable set cell in CellForest: Passing a position without any "
          "axis");
      return true;
    }

    return false;
  }

  bool IsAreaInvalid(const Area& area) const {
    CELLNTA_PROFILE;

    if (!area.Valid()) {
      CELLNTA_LOG_ERROR(
          "Unable to erase area in CellForest: Passing a invalid area");
      return true;
    }

    return false;
  }

  void SetNodeState(Node& node, CellState state) {
    CELLNTA_PROFILE;

    if (node.state == 0 && state != 0)
      m_size += 1;
    if (node.state != 0 && state == 0)
      m_size -= 1;
    node.state = state;
  }

  template <typename T,
            typename Iterator = typename std::conditional<
                std::is_const<T>::value, MapConstIter, MapIter>::type>
  Iterator FindChildIter(T*& parent, const Pos& pos,
                         int* iterReporterOnError = nullptr) const {
    CELLNTA_PROFILE;

    Iterator iter;
    T* child = parent;
    for (int i = 0; i < pos.size(); ++i) {
      parent = child;

      iter = child->find(pos[i]);
      if (iter == child->end()) {
        if (iterReporterOnError != nullptr)
          *iterReporterOnError = i;

        return iter;
      }

      child = &iter->second.child;
    }

    return iter;
  }

  Node& GetChildNode(const Pos& pos) {
    CELLNTA_PROFILE;

    int i = 0;
    Map* parent = &m_roots;
    MapIter iter = FindChildIter(parent, pos, &i);

    if (iter == parent->end()) {
      for (; i < pos.size(); ++i) {
        iter = CreateNode(*parent, pos[i]);
        parent = &iter->second.child;
      }
    }

    return iter->second;
  }

  MapIter GetRootIter(Scalar rootAxis) {
    CELLNTA_PROFILE;

    auto res = m_roots.find(rootAxis);
    if (res != m_roots.end()) {
      return res;
    }

    return CreateNode(m_roots, rootAxis);
  }

  MapIter CreateNode(Map& cont, Scalar axisVal) {
    CELLNTA_PROFILE;

    auto res = cont.emplace(axisVal, Node());
    m_capacity += 1;

    return res.first;
  }

  MapIter DeleteNode(Map& cont, MapIter& iter) {
    CELLNTA_PROFILE;

    assert(iter != cont.end() && "Iteartor must be valid when deleting node");

    Node& node = iter->second;
    if (!node.child.empty()) {
      SetNodeState(node, 0);
      return ++iter;
    }

    if (node.state != 0)
      m_size -= 1;
    m_capacity -= 1;

    return cont.erase(iter);
  }

  template <typename Iterator = MapConstIter>
  struct IterRange {
    Iterator it;
    const Iterator end;

    IterRange(Iterator&& it, const Iterator&& end)
        : it(std::move(it)), end(std::move(end)) {}
  };

  template <typename Derived, typename TDetermineRange>
  struct IterCommon {
    IterCommon(const CellForest& tree) : m_tree(&tree) {}

    void Reset() {
      m_stack = std::stack<IterRange<>>();
      PushToStack(m_tree->m_roots);
    }

    const Cell* Next() {
      CELLNTA_PROFILE;

      if (m_stack.empty())
        return nullptr;

      IterRange<>& curr = m_stack.top();
      if (curr.it == curr.end) {
        m_stack.pop();
        return Next();
      }

      if (m_cell.pos.size() != (int)m_stack.size()) {
        m_cell.pos.conservativeResize(m_stack.size());
      }

      const auto& axisVal = curr.it->first;
      const auto& node = curr.it->second;
      m_cell.pos[m_stack.size() - 1] = axisVal;
      curr.it++;

      if (!node.child.empty())
        PushToStack(node.child);

      if (node.state == 0)
        return Next();

      m_cell.state = node.state;
      return &m_cell;
    }

   private:
    void PushToStack(const Map& map) {
      IterRange<> range = TDetermineRange()(GetDerived(), map, m_stack.size());

      m_stack.push(std::move(range));
    }
    Derived& GetDerived() { return *(Derived*)(this); }

   protected:
    std::stack<IterRange<>> m_stack;
    Cell m_cell;
    const CellForest* m_tree = nullptr;
  };

  struct DetermineWholeIterRange {
    IterRange<> operator()(const WholeIter&, const Map& node, int) {
      return IterRange<>(node.begin(), node.end());
    }
  };

  class WholeIter : public IterBase::CellForwardEx<Cell>,
                    protected IterCommon<WholeIter, DetermineWholeIterRange> {
    using ItCommon = IterCommon<WholeIter, DetermineWholeIterRange>;

   public:
    WholeIter(const CellForest& tree) : ItCommon(tree) { Reset(); }

    void Reset() override { ItCommon::Reset(); }

    const Cell* Next() override { return ItCommon::Next(); }
  };

  struct DetermineAreaIterRange {
    IterRange<> operator()(const AreaIter& iter, const Map& node, int i) {
      auto min = node.lower_bound(Axis::NotLess(iter.GetArea().MinAxis(i)));
      auto max = node.upper_bound(Axis::GreaterThan(iter.GetArea().MaxAxis(i)));
      return IterRange<>(std::move(min), std::move(max));
    }
  };

  class AreaIter : public IterBase::CellForwardEx<Cell>,
                   protected IterCommon<AreaIter, DetermineAreaIterRange> {
    using ItCommon = IterCommon<AreaIter, DetermineAreaIterRange>;

   public:
    AreaIter(const CellForest& tree, const Area& area)
        : ItCommon(tree), m_area(area) {
      Reset();
    }

    void Reset() override {
      if (!m_area.Valid()) {
        CELLNTA_LOG_WARN(
            "Unable to reset CellForest::AreaIter: Passed invalid area");
        return;
      }

      ItCommon::Reset();
    }

    const Cell* Next() override { return ItCommon::Next(); }

    const Area& GetArea() const { return m_area; }

   private:
    Area m_area;
  };

  struct Node {
    CellState state = 0;
    Map child;  // TODO: Maybe use cache friendly approach?

    Node() = default;
    Node(CellState state) : state(state) {}
  };

  Map m_roots;
  int m_dim = 0;
  int m_size = 0;
  int m_capacity = 0;
};

}  // namespace Cellnta
