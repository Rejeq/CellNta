#pragma once

#include <Eigen/Core>

#include <imgui.h>

namespace Ui {
namespace Widget {

template <typename Derived>
bool DragMatrix(const char* id, Eigen::MatrixBase<Derived>& mat) {
  constexpr float ItemWidth = 75.0f;
  constexpr float DragMin = 0.1f;

  bool updated = false;

  ImGui::PushID(id);
  for (uint32_t r = 0; r < mat.rows(); ++r) {
    ImGui::PushID(r);
    for (uint32_t c = 0; c < mat.cols(); ++c) {
      ImGui::PushID(c);
      float& data = mat(r, c);
      ImGui::SetNextItemWidth(ItemWidth);
      if (ImGui::DragFloat("", &data, DragMin))
        updated = true;
      if (c != mat.cols() - 1)
        ImGui::SameLine();
      ImGui::PopID();
    }
    ImGui::PopID();
  }
  ImGui::PopID();

  return updated;
}

template <typename Derived>
void TextMatrix(const Eigen::MatrixBase<Derived>& mat) {
  std::stringstream ss;
  ss.setf(std::ios::fixed);
  ss << std::setprecision(3) << mat;

  ImGui::Text("%s", ss.str().c_str());
}
}  // namespace Widget
}  // namespace Ui
