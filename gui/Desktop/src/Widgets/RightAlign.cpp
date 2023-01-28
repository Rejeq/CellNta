#include "RightAlign.h"

#include <cassert>

#include <imgui.h>

#include <Cellnta/Config.h>

using namespace Ui;
using namespace Ui::Widget;

void RightAlign::Begin(int right) {
  CELLNTA_PROFILE;

  m_currentIdx = 1;
  PushPosition();
  m_pos[0] = right;
}

void RightAlign::End() {
  CELLNTA_PROFILE;

  SavePosition();
}

void RightAlign::Next() {
  CELLNTA_PROFILE;

  assert(m_pos.size() != 0 &&
         "Begin() must be called. Positions does not allocated");

  SavePosition();
  m_currentIdx += 1;
  PushPosition();
}

void RightAlign::SavePosition() {
  CELLNTA_PROFILE;

  float width = ImGui::GetCursorPosX() - m_pos[m_currentIdx];
  m_pos[m_currentIdx] = m_pos[m_currentIdx - 1] - width;
}

void RightAlign::PushPosition() {
  CELLNTA_PROFILE;

  if (m_currentIdx + 1 >= m_pos.size())
    m_pos.resize(m_currentIdx + 1, -1);

  if (m_pos[m_currentIdx] != -1)
    ImGui::SetCursorPosX(m_pos[m_currentIdx]);
}
