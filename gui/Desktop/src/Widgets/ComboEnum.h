#pragma once

#include <array>

#include <imgui.h>

namespace Ui {

template <typename E>
struct ComboData {
  E index;
  const char* name = nullptr;

  ComboData(E index, const char* name) : index{index}, name(name) {}

  bool operator==(const E& l) const { return index == l; }
  bool operator==(const ComboData<E>& l) const { return index == l.index; }
};

namespace Widget {

template <typename E, size_t N>
bool ComboEnum(const char* label, E selected,
               const std::array<ComboData<E>, N>& data, E& out) {
  const auto& res = std::find(data.begin(), data.end(), selected);
  bool updated = false;

  if (res == data.end())
    return false;

  if (ImGui::BeginCombo(label, res->name)) {
    for (const auto& item : data) {
      const bool is_selected = (item == selected);

      if (ImGui::Selectable(item.name, is_selected)) {
        out = item.index;
        updated = true;
      }
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  return updated;
}

}  // namespace Widget
}  // namespace Ui
