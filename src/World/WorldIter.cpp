#include "Cellnta/World/WorldIter.h"

#include "Cellnta/Log.h"

using namespace Cellnta;

WorldIter::WorldIter(ImplRef&& impl) : m_impl(std::move(impl)) {
  if (m_impl == nullptr)
    CELLNTA_LOG_ERROR("Unable to create WorldIter: Passing a nullptr impl");
}

void WorldIter::Reset() {
  if (m_impl == nullptr) {
    CELLNTA_LOG_ERROR("Unable to Reset() WorldIter: impl was not properly created");
    return;
  }
  m_impl->Reset();
}

const Cell* WorldIter::Next() {
  if (m_impl == nullptr) {
    CELLNTA_LOG_ERROR("Unable to Next() WorldIter: impl was not properly created");
    return nullptr;
  }
  return m_impl->Next();
}
