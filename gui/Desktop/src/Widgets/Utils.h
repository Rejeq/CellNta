#pragma once

#include <boost/dynamic_bitset.hpp>

#include <imgui.h>

#include <Cellnta/Cell.h>

namespace Ui {
namespace Widget {

template <typename T>
constexpr ImGuiDataType GetImGuiDataType();

void Separator();
bool ToggleButton(const char* label, bool& v);

void PositionSelector(size_t d, Cellnta::Cell::Pos& pos);
void CellSelector(size_t d, Cellnta::Cell& cell);
void CellSelectorHomogeneous(size_t d, Cellnta::Cell& cell);

bool RuleMask(const char* label, boost::dynamic_bitset<>& mask);

template <typename T>
inline bool Drag(const char* label, T* p_data, const float speed = 1.0f,
                 ImGuiInputTextFlags flags = 0) {
  return ImGui::DragScalar(label, GetImGuiDataType<T>(), (void*)p_data, speed,
                           nullptr, nullptr, nullptr, flags);
}

template <typename T>
inline bool DragN(const char* label, T* p_data, int components,
                  const float speed = 1.0f, ImGuiInputTextFlags flags = 0) {
  return ImGui::DragScalarN(label, GetImGuiDataType<T>(), (void*)p_data,
                            components, speed, nullptr, nullptr, nullptr,
                            flags);
}

template <typename T, typename J = T>
inline bool Input(const char* label, T* p_data, const J step = (J)1,
                  ImGuiInputTextFlags flags = 0) {
  return ImGui::InputScalar(label, GetImGuiDataType<T>(), (void*)p_data, &step,
                            nullptr, nullptr, flags);
}
template <typename T, typename J = T>
inline bool InputN(const char* label, T* p_data, int components,
                   const J step = (J)1, ImGuiInputTextFlags flags = 0) {
  return ImGui::InputScalarN(label, GetImGuiDataType<T>(), (void*)p_data,
                             components, &step, nullptr, nullptr, flags);
}

template <typename T>
constexpr ImGuiDataType GetImGuiDataType() {
  static_assert(!sizeof(T), "Cannot convert T to ImGuiDataType");
  return 0;
}

template <>
constexpr ImGuiDataType GetImGuiDataType<double>() {
  return ImGuiDataType_Double;
}
template <>
constexpr ImGuiDataType GetImGuiDataType<float>() {
  return ImGuiDataType_Float;
}
template <>
constexpr ImGuiDataType GetImGuiDataType<uint8_t>() {
  return ImGuiDataType_U8;
}
template <>
constexpr ImGuiDataType GetImGuiDataType<uint16_t>() {
  return ImGuiDataType_U16;
}
template <>
constexpr ImGuiDataType GetImGuiDataType<uint32_t>() {
  return ImGuiDataType_U32;
}
template <>
constexpr ImGuiDataType GetImGuiDataType<uint64_t>() {
  return ImGuiDataType_U64;
}
template <>
constexpr ImGuiDataType GetImGuiDataType<int8_t>() {
  return ImGuiDataType_S8;
}
template <>
constexpr ImGuiDataType GetImGuiDataType<int16_t>() {
  return ImGuiDataType_S16;
}
template <>
constexpr ImGuiDataType GetImGuiDataType<int32_t>() {
  return ImGuiDataType_S32;
}
template <>
constexpr ImGuiDataType GetImGuiDataType<int64_t>() {
  return ImGuiDataType_S64;
}
}  // namespace Widget
}  // namespace Ui
