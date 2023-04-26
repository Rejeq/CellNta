#pragma once

#include "Cellnta/Iterator.h"

namespace Cellnta {

class WorldIter : public IterBase::CellForward {
 public:
  using ImplRef = std::unique_ptr<IterBase::CellForward>;

  WorldIter(ImplRef&& impl);

  void Reset() override;
  const Cell* Next() override;

  template <typename Iter>
  static ImplRef MakeImpl(Iter&& iter) {
    return std::make_unique<Iter>(std::move(iter));
  }

 private:
  ImplRef m_impl;
};

}  // namespace Cellnta
