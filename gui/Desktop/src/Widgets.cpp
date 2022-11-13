#include "Widgets.h"

#include "imgui_stdlib.h"

#include "Context.h"

using namespace Ui;
using namespace Ui::Widget;

void Widget::Separator()
{
  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();
}

bool Widget::ToggleButton(const char* label, bool& v)
{
  const bool lastV = v;
  bool out = false;

  if (lastV)
  {
    ImVec4& activeColor = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
    ImGui::PushStyleColor(ImGuiCol_Button, activeColor);
  }

  if (ImGui::Button(label))
  {
    v = !v;
    out = true;
  }

  if (lastV)
    ImGui::PopStyleColor();
  return out;
}

bool Widget::RuleMask(const char* label , boost::dynamic_bitset<>& mask)
{
  auto FilterOnlyNums = [](ImGuiInputTextCallbackData* data) -> int
  {
    bool out = false;
    if (!(data->EventChar >= '0' && data->EventChar <= '9'))
      out = true;

    return out;
  };

  constexpr ImGuiTextFlags flags = ImGuiInputTextFlags_CallbackCharFilter;

  //TODO: add support several digits
  std::string str;

  for (size_t i = mask.find_first(); i != boost::dynamic_bitset<>::npos; i = mask.find_next(i))
    str.push_back(i + '0');

  bool out = ImGui::InputText(label, &str, flags, FilterOnlyNums);

  if (out)
  {
    if (str.size() >= mask.size())
      return false;

    //Strings with the same values but in different order are equal
    //so we need to exclude false positives
    boost::dynamic_bitset<> oldMask = mask;
    mask.reset();
    for (auto ch : str)
    {
      if (ch >= '0' && ch <= '9')
        mask[ch - '0'] = ch - '0';
    }

    if(mask == oldMask)
      out = false;
  }
  return out;
}
