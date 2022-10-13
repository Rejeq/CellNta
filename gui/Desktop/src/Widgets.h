#pragma once

#include <iomanip>
#include <boost/dynamic_bitset.hpp>

#include <Eigen/Core>

#include <imgui.h>
#include <imgui_internal.h>


#include "AlgoBase.h"

namespace Ui
{
  template<typename E>
  struct ComboData
  {
    E index;
    const char* name = nullptr;

    ComboData(E index, const char* name): index{index}, name(name) {}
    bool operator==(const E& l) const { return index == l; }
    bool operator==(const ComboData<E>& l) const { return index == l.index; }
  };

namespace Widget
{
  template<typename T>
  constexpr ImGuiDataType GetImGuiDataType();

  void Separator();
  bool ToggleButton(const char* label, bool& v);

  template<bool homogeneous = false>
  void CellSelector(size_t d, Lf::Cell& cell);

  bool RuleMask(const char* label, boost::dynamic_bitset<>& mask);

  template<typename Derived>
  bool DragMatrix(const char* id, Eigen::MatrixBase<Derived>& mat);

  template<typename Derived>
  void TextMatrix(const Eigen::MatrixBase<Derived>& mat);

  template<typename E, size_t N>
  bool ComboEnum(const char* label, const E selected, const std::array<ComboData<E>, N>& data, E& out);

  template<typename T>
  inline bool Drag(const char* label, T* p_data, const float speed = 1.0f, ImGuiInputTextFlags flags = 0) {
    return ImGui::DragScalar(label, GetImGuiDataType<T>(), (void*)p_data, speed, nullptr, nullptr, nullptr, flags);
  }

  template<typename T>
  inline bool DragN(const char* label, T* p_data, int components, const float speed = 1.0f, ImGuiInputTextFlags flags = 0) {
    return ImGui::DragScalarN(label, GetImGuiDataType<T>(), (void*)p_data, components, speed, nullptr, nullptr, nullptr, flags);
  }

  template<typename T, typename J = T>
  inline bool Input(const char* label, T* p_data, const J step = (J)1, ImGuiInputTextFlags flags = 0) {
    return ImGui::InputScalar(label, GetImGuiDataType<T>(), (void*)p_data, &step, nullptr, nullptr, flags);
  }
  template<typename T, typename J = T>
  inline bool InputN(const char* label, T* p_data, int components, const J step = (J)1, ImGuiInputTextFlags flags = 0) {
    return ImGui::InputScalarN(label, GetImGuiDataType<T>(), (void*)p_data, components, &step, nullptr, nullptr, flags);
  }



  template<bool homogeneous>
  void CellSelector(size_t d, Lf::Cell& cell)
  {
    const int vecSize = (homogeneous) ? d + 1 : d;

    if (cell.pos.size() != vecSize)
    {
      Lf::Cell::Vec tmp = Lf::Cell::Vec::Zero(vecSize);
      const int minSize = std::min(cell.pos.size(), tmp.size());
      memcpy(tmp.data(), cell.pos.data(), minSize * sizeof(Lf::Cell::Vec::Scalar));

      if constexpr (homogeneous)
      {
        if (cell.pos.size() < tmp.size())
          tmp(d - 1) = 0.0f;
        tmp(d) = 1.0f;
      }
      cell.pos = tmp;
    }

    DragN("Position ##CellSelector", cell.pos.data(), cell.pos.size());
    Input("State##CellSelector", &cell.state);
  }

  template<typename Derived>
  bool DragMatrix(const char* id, Eigen::MatrixBase<Derived>& mat)
  {
    constexpr float ItemWidth = 75.0f;
    constexpr float DragMin = 0.1f;

    bool updated = false;

    ImGui::PushID(id);
    for (uint32_t r = 0; r < mat.rows(); ++r)
    {
      ImGui::PushID(r);
      for (uint32_t c = 0; c < mat.cols(); ++c)
      {
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

  template<typename Derived>
  void TextMatrix(const Eigen::MatrixBase<Derived>& mat)
  {
    std::stringstream ss;
    ss.setf(std::ios::fixed);
    ss << std::setprecision(3) << mat;

    ImGui::Text("%s", ss.str().c_str());
  }

  template<typename E, size_t N>
  bool ComboEnum(const char* label, const E selected, const std::array<ComboData<E>, N>& data, E& out)
  {
    const auto& res = std::find(data.begin(), data.end(), selected);
    bool updated = false;

    if (res == data.end())
      return false;

    if (ImGui::BeginCombo(label, res->name))
    {
      for(const auto& item: data)
      {
        const bool is_selected = (item == selected);

        if (ImGui::Selectable(item.name, is_selected))
        {
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

  template<typename T>
  constexpr ImGuiDataType GetImGuiDataType() { static_assert(!sizeof(T), "Cannot convert T to ImGuiDataType"); return 0;}
  template<> constexpr ImGuiDataType GetImGuiDataType<double>()   { return ImGuiDataType_Double; }
  template<> constexpr ImGuiDataType GetImGuiDataType<float>()    { return ImGuiDataType_Float; }
  template<> constexpr ImGuiDataType GetImGuiDataType<uint8_t>()  { return ImGuiDataType_U8; }
  template<> constexpr ImGuiDataType GetImGuiDataType<uint16_t>() { return ImGuiDataType_U16; }
  template<> constexpr ImGuiDataType GetImGuiDataType<uint32_t>() { return ImGuiDataType_U32; }
  template<> constexpr ImGuiDataType GetImGuiDataType<uint64_t>() { return ImGuiDataType_U64; }
  template<> constexpr ImGuiDataType GetImGuiDataType<int8_t>()   { return ImGuiDataType_S8; }
  template<> constexpr ImGuiDataType GetImGuiDataType<int16_t>()  { return ImGuiDataType_S16; }
  template<> constexpr ImGuiDataType GetImGuiDataType<int32_t>()  { return ImGuiDataType_S32; }
  template<> constexpr ImGuiDataType GetImGuiDataType<int64_t>()  { return ImGuiDataType_S64; }
}
}
