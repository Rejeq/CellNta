#include "Utils.h"

#include "imgui_stdlib.h"

using namespace Ui;
using namespace Ui::Widget;

void Widget::Separator() {
  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();
}

bool Widget::ToggleButton(const char* label, bool& v) {
  const bool lastV = v;
  bool out = false;

  if (lastV) {
    ImVec4& activeColor = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
    ImGui::PushStyleColor(ImGuiCol_Button, activeColor);
  }

  if (ImGui::Button(label)) {
    v = !v;
    out = true;
  }

  if (lastV)
    ImGui::PopStyleColor();
  return out;
}

static void PositionSelectorEx(size_t targetSize, Cellnta::Cell::Pos& pos,
                               bool shiftLastPoint) {
  const size_t realSize = pos.size();

  if (realSize != targetSize) {
    Cellnta::Cell::Pos tmp = Cellnta::Cell::Pos::Zero(targetSize);
    const int minSize = std::min(realSize, targetSize);
    memcpy(tmp.data(), pos.data(), minSize * sizeof(Cellnta::Cell::Point));

    if (shiftLastPoint) {
      if (realSize < targetSize && realSize != 0)
        tmp(realSize - 1) = 0.0f;
      tmp(targetSize - 1) = 1.0f;
    }
    pos = tmp;
  }

  ImGui::PushID("PositionSelector");
  Widget::DragN("Position", pos.data(), pos.size());
  ImGui::PopID();
}

void Widget::PositionSelector(size_t d, Cellnta::Cell::Pos& pos) {
  PositionSelectorEx(d, pos, false);
}

void Widget::CellSelector(size_t d, Cellnta::Cell& cell) {
  ImGui::PushID("CellSelector");
  PositionSelectorEx(d, cell.pos, false);
  Widget::Input("State", &cell.state);
  ImGui::PopID();
  // CellSelectorEx(d, cell, false);
}

void Widget::CellSelectorHomogeneous(size_t d, Cellnta::Cell& cell) {
  ImGui::PushID("CellSelector");
  PositionSelectorEx(d + 1, cell.pos, true);
  Widget::Input("State", &cell.state);
  ImGui::PopID();
  // CellSelectorEx(d + 1, cell, true);
}

bool Widget::RuleMask(const char* label, boost::dynamic_bitset<>& mask) {
  std::string maskStr;

  for (size_t i = mask.find_first(); i != boost::dynamic_bitset<>::npos;
       i = mask.find_next(i))
    maskStr.push_back(i + '0');

  bool changed =
      ImGui::InputText(label, &maskStr, ImGuiInputTextFlags_CharsDecimal);

  if (changed) {
    if (maskStr.size() >= mask.size())
      return false;

    // Strings with the same values but in different order are equal
    // so we need to exclude false positives
    boost::dynamic_bitset<> oldMask = mask;
    mask.reset();
    // FIXME: Currently only single digits can be entered
    // For example, we cannot set 12 bits
    // it will be treated as 1 and 2
    for (auto ch : maskStr) {
      if (ch >= '0' && ch <= '9')
        mask[ch - '0'] = ch - '0';
    }

    if (mask == oldMask)
      return false;
  }
  return changed;
}
