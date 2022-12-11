#include "RendererWindow.h"

#include <array>
//TODO: Need use external fmt
#include <spdlog/fmt/fmt.h>

#include <Cellnta/Renderer/Camera3d.h>
#include <Cellnta/Renderer/CameraNd.h>

#include "Context.h"
#include "Widgets.h"

using namespace Ui;

void RendererWindow::Draw() {
  CELLNTA_PROFILE;

  constexpr ImGuiWindowFlags WinFlags = ImGuiWindowFlags_HorizontalScrollbar;
  Cellnta::Renderer& ren = GetContext()->GetRenderer();

  if (ImGui::Begin(p_prop.Name, &p_prop.Opened, WinFlags)) {
    uint32_t dimensions = ren.GetDimensions();
    if (Widget::Input("Cube dimensions", &dimensions, 1,
                      ImGuiInputTextFlags_CharsDecimal)) {
      ren.SetDimension(dimensions);
      ren.GenrateHypercube(0.5f);
    }

    uint32_t renderDistance = ren.GetData().GetDistance();
    // TODO: from keyboard input negative number not clamped
    if (Widget::Input("Render distance", &renderDistance, 1,
                      ImGuiInputTextFlags_CharsDecimal))
      ren.SetRenderDistance(renderDistance);

    static const std::array<ComboData<Cellnta::CubeMode>, 3> CubeModeData = {
        ComboData(Cellnta::CubeMode::POINTS, "Points"),
        ComboData(Cellnta::CubeMode::WIREFRAME, "Wireframe"),
        ComboData(Cellnta::CubeMode::POLYGON, "Polygon (gives errors from 6d)"),
    };

    Cellnta::CubeMode res = Cellnta::CubeMode::NONE;
    if (Widget::ComboEnum("Cube mode", ren.GetCubeMode(), CubeModeData, res))
      ren.SetCubeMode(res);

    static Cellnta::Cell cell;
    Widget::CellSelector<true>(ren.GetDimensions(), cell);
    ImGui::Spacing();
    if (ImGui::Button("Add cell (Only for renderer)"))
      ren.AddCell(cell.pos);

    ShowCollatingInfo();

    ImGui::Spacing();

    if (ImGui::BeginTabBar("RendererTabBar")) {
      if (ImGui::BeginTabItem("Dimensions cameras")) {
        ImGui::Spacing();
        DrawCameras();
        ImGui::EndTabItem();
      }

      {
        const size_t cellsSize = ren.GetCells().size();
        if (cellsSize != 0)
          m_showCellsTab = true;

        size_t size = 28 + 9;
        char* CellsTabStr = GetContext()->GetTmpBuffer(size);
        snprintf(CellsTabStr, size, "Loaded Cells(%zu)###LoadedCells",
                 cellsSize);

        if (m_showCellsTab) {
          if (ImGui::BeginTabItem(CellsTabStr)) {
            ImGui::Spacing();
            DrawLoadedCells();
            ImGui::EndTabItem();
          }
        }
      }

      ImGui::EndTabBar();
    }
  }

  ImGui::End();
}

void RendererWindow::ShowCollatingInfo() {
  Cellnta::Renderer& ren = GetContext()->GetRenderer();

  int x = ren.GetCollatingX();
  int y = ren.GetCollatingY();
  int z = ren.GetCollatingZ();
  if (ImGui::InputInt("X", &x))
    ren.SetCollatingX(x);
  if (ImGui::InputInt("Y", &y))
    ren.SetCollatingY(y);
  if (ImGui::InputInt("Z", &z))
    ren.SetCollatingZ(z);
}

void RendererWindow::DrawLoadedCells() {
  CELLNTA_PROFILE;

  Cellnta::Renderer& ren = GetContext()->GetRenderer();
  const Cellnta::NCellStorage& cells = ren.GetCells();
  ImGui::Text("Loaded: %d", cells.size());

  DrawCells(cells);
}

void RendererWindow::DrawCells(const Cellnta::NCellStorage& cells) {
  constexpr ImGuiTableFlags TableFlags =
      0 | ImGuiTableFlags_Borders | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY |
      ImGuiTableFlags_Hideable;

  if (ImGui::BeginTable("tableCells", 3, TableFlags)) {
    ImGui::TableSetupColumn(
        "#", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_NoHide);
    ImGui::TableSetupColumn("Original", ImGuiTableColumnFlags_NoResize);
    ImGui::TableSetupColumn("Shown", ImGuiTableColumnFlags_NoResize);
    ImGui::TableSetupScrollFreeze(1, 1);

    ImGuiTable* table = ImGui::GetCurrentTable();
    ImGui::TableHeadersRow();

    const ImU32 cellBg =
        ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_TableRowBg]);
    const ImU32 cellBgAlt =
        0x25000000  // Increases alpha channel
        + ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_TableRowBgAlt]);

    ImGuiListClipper clipper(cells.size());
    while (clipper.Step()) {
      for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
        const Cellnta::NCellStorage::Vec& OrigCell = cells.OriginalAt(i);
        const Cellnta::NCellStorage::Vec& cell = cells.at(i);

        ImGui::PushID(i);

        ImGui::TableNextColumn();
        ImGui::Text("%i", i);

        ImGui::TableNextColumn();
        if (i % 2 == 0)
          ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cellBg);
        else
          ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cellBgAlt);
        Widget::TextMatrix(OrigCell.transpose());

        ImGui::TableNextColumn();
        if (i % 2 == 0)
          ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cellBgAlt);
        else
          ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cellBg);
        Widget::TextMatrix(cell.transpose());

        ImGui::PopID();
      }
    }
    ImGui::EndTable();
  }
}

void RendererWindow::DrawCameras() {
  CELLNTA_PROFILE;

  Cellnta::Renderer& ren = GetContext()->GetRenderer();
  Cellnta::Camera3d* camera3d = ren.GetCamera3d();
  Cellnta::CameraNdList* cameraNd = ren.GetCameraNd();

  if (camera3d != nullptr)
    DrawCameraSensitivity(*camera3d);

  if(cameraNd != nullptr)
    DrawCameraNdList(*cameraNd);

  if (camera3d != nullptr) {
    if (ImGui::TreeNodeEx("3d camera", ImGuiTreeNodeFlags_DefaultOpen)) {
      DrawCamera3d(*camera3d);
      ImGui::TreePop();
    }
  }
}


void RendererWindow::DrawCameraSensitivity(Cellnta::Camera3d& camera) {
  CELLNTA_PROFILE;

  float mouseSpeed = camera.GetMouseSpeed();
  if (ImGui::DragFloat("Mouse sensitivity", &mouseSpeed, 0.001f, 0.0f, 100.0f))
    camera.SetMouseSpeed(mouseSpeed);

  float moveSpeed = camera.GetMoveSpeed();
  if (ImGui::DragFloat("Move speed", &moveSpeed, 0.001f, 0.0f, 100.0f))
    camera.SetMoveSpeed(moveSpeed);

}

void RendererWindow::DrawCamera3d(Cellnta::Camera3d& cam3d) {
  CELLNTA_PROFILE;

  ImGui::PushID(3); // 3 is dimension

  Eigen::VectorXf pos = cam3d.GetPosition();
  Eigen::VectorXf front = cam3d.GetFront();
  if (ImGui::DragFloat3("Position##CameraPos", pos.data(), 0.01f))
    cam3d.SetPosition(pos);

  constexpr float ItemWidth = 75.0f;
  constexpr float DragRotateSpeed = 0.002f;

  float yaw = cam3d.GetYaw();
  ImGui::SetNextItemWidth(ItemWidth);
  if (ImGui::DragFloat("Yaw##Camera", &yaw, DragRotateSpeed))
    cam3d.SetYaw(yaw);

  ImGui::SameLine();

  float pitch = cam3d.GetPitch();
  ImGui::SetNextItemWidth(ItemWidth);
  if (ImGui::DragFloat("Pitch##Camera", &pitch, DragRotateSpeed))
    cam3d.SetPitch(pitch);

  ImGui::Spacing();

  DrawCameraController(cam3d);

  ImGui::PopID();
}

void RendererWindow::DrawCameraNdList(Cellnta::CameraNdList& list) {
  CELLNTA_PROFILE;

  for (Cellnta::CameraNd& cam : list) {
    std::string camName = fmt::format("{} Camera", cam.GetDimensions());

    if (ImGui::TreeNodeEx(camName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
      DrawCameraNd(cam);
      ImGui::TreePop();
    }
  }
}

void RendererWindow::DrawCameraNd(Cellnta::CameraNd& camNd) {
  CELLNTA_PROFILE;

  ImGui::PushID(camNd.GetDimensions());

  bool skip = camNd.WantSkip();
  if (ImGui::Checkbox("Skip", &skip))
    camNd.NeedSkip(skip);

  if (skip)
    ImGui::BeginDisabled();

  constexpr float DragSpeed = 0.01f;

  Eigen::VectorXf pos = camNd.GetPosition();
  Eigen::VectorXf front = camNd.GetFront();

  if (Widget::DragN("Position##CameraPos", pos.data(), pos.size(), DragSpeed))
    camNd.SetPosition(pos);

  if (Widget::DragN("Target##CameraPos", front.data(), front.size(), DragSpeed))
    camNd.SetFront(front);

  ImGui::Spacing();

  DrawCameraController(camNd);

  if (skip)
    ImGui::EndDisabled();

  ImGui::PopID();
}

void RendererWindow::DrawCameraController(Cellnta::CameraController& controller) {
  constexpr const char* PerspectiveStr = "Current perspective";
  constexpr const char* OrthoStr = "Current orthographic";

  bool perspective = controller.GetUsePerspective();
  const char* perspectiveName = (perspective) ? PerspectiveStr : OrthoStr;

  if (ImGui::Checkbox(perspectiveName, &perspective))
    controller.SetUsePerspective(perspective);
}
