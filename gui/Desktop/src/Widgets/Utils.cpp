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
  using Pos = Cellnta::Cell::Pos;

  const size_t realSize = pos.size();

  if (realSize != targetSize) {
    Pos tmp = Pos::Zero(targetSize);
    const int minSize = std::min(realSize, targetSize);
    memcpy(tmp.data(), pos.data(), minSize * sizeof(Pos::Scalar));

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
