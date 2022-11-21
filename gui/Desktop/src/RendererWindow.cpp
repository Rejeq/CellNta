#include "RendererWindow.h"

#include <array>

#include "Widgets.h"
#include "Context.h"

void Ui::RendererWindow::Draw()
{
  CELLNTA_PROFILE;

  constexpr ImGuiWindowFlags WinFlags = ImGuiWindowFlags_HorizontalScrollbar;
  Cellnta::Renderer& ren = GetContext()->GetCanvas();

  if (ImGui::Begin(p_prop.Name, &p_prop.Opened, WinFlags))
  {
    uint32_t dimensions = ren.GetDimensions();
    if (Widget::Input("Cube dimensions", &dimensions, 1, ImGuiInputTextFlags_CharsDecimal))
    {
      ren.SetDimension(dimensions);
      ren.GenrateHypercube(0.5f);
    }

    uint32_t renderDistance = ren.GetData().GetDistance();
    //TODO: from keyboard input negative number not clamped
    if (Widget::Input("Render distance", &renderDistance, 1, ImGuiInputTextFlags_CharsDecimal))
      ren.SetRenderDistance(renderDistance);

    static const std::array<ComboData<Cellnta::CubeMode>, 3> CubeModeData = {
      ComboData(Cellnta::CubeMode::POINTS,    "Points"),
      ComboData(Cellnta::CubeMode::WIREFRAME, "Wireframe"),
      ComboData(Cellnta::CubeMode::POLYGON,   "Polygon (gives errors from 6d)"),
    };

    Cellnta::CubeMode res;
    if (Widget::ComboEnum("Cube mode", ren.GetCubeMode(), CubeModeData, res))
      ren.SetCubeMode(res);

    static Cellnta::Cell cell;
    Widget::CellSelector<true>(ren.GetDimensions(), cell);
    ImGui::Spacing();
    if (ImGui::Button("Add cell (Only for renderer)"))
      ren.AddCell(cell.pos);

    ShowCollatingInfo();

    ImGui::Spacing();

    if (ImGui::BeginTabBar("RendererTabBar"))
    {
      if (ImGui::BeginTabItem("Dimensions cameras"))
      {
        ImGui::Spacing();
        DrawCameras();
        ImGui::EndTabItem();
      }

      {
        const size_t cellsSize = ren.GetCells().size();
        if (cellsSize != 0) m_showCellsTab = true;

        size_t size = 28 + 9;
        char* CellsTabStr = GetContext()->GetTmpBuffer(size);
        snprintf(CellsTabStr, size, "Loaded Cells(%zu)###LoadedCells", cellsSize);

        if (m_showCellsTab)
        {
          if (ImGui::BeginTabItem(CellsTabStr))
          {
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

void Ui::RendererWindow::ShowCollatingInfo()
{
  Cellnta::Renderer& ren = GetContext()->GetCanvas();

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

void Ui::RendererWindow::DrawCameras()
{
  CELLNTA_PROFILE;

  Cellnta::Renderer& ren = GetContext()->GetCanvas();
  Cellnta::Camera3d& camera3d = ren.GetCamera3d();

  float mouseSpeed = camera3d.GetMouseSpeed();
  if (ImGui::DragFloat("Mouse sensitivity", &mouseSpeed, 0.001f, 0.0f, 100.0f))
    camera3d.SetMouseSpeed(mouseSpeed);

  float moveSpeed = camera3d.GetMoveSpeed();
  if (ImGui::DragFloat("Move speed", &moveSpeed, 0.001f, 0.0f, 100.0f))
    camera3d.SetMoveSpeed(moveSpeed);

  for (Cellnta::CameraNd& cam : ren.GetNdCameras())
  {
    size_t size = 8 + 3;
    char* fmt = GetContext()->GetTmpBuffer(size);
    snprintf(fmt, size, "%zud Camera", cam.GetDimensions());

    if (ImGui::TreeNodeEx(fmt, ImGuiTreeNodeFlags_DefaultOpen))
    {
      PrintCameraNd(cam);
      ImGui::TreePop();
    }
  }

  if (ImGui::TreeNodeEx("3d camera", ImGuiTreeNodeFlags_DefaultOpen))
  {
    PrintCamera3d(camera3d);
    ImGui::TreePop();
  }
}

void Ui::RendererWindow::DrawLoadedCells()
{
  CELLNTA_PROFILE;

  Cellnta::Renderer& ren = GetContext()->GetCanvas();
  const Cellnta::NCellStorage& cells = ren.GetCells();
  ImGui::Text("Loaded: %zu", cells.size());

  DrawCells(cells);
}

void Ui::RendererWindow::DrawCells(const Cellnta::NCellStorage& cells)
{
  constexpr ImGuiTableFlags TableFlags = 0
    | ImGuiTableFlags_Borders | ImGuiTableFlags_Reorderable
    | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
    | ImGuiTableFlags_Hideable;

  if (ImGui::BeginTable("tableCells", 3, TableFlags))
  {
    ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_NoHide);
    ImGui::TableSetupColumn("Original", ImGuiTableColumnFlags_NoResize);
    ImGui::TableSetupColumn("Shown", ImGuiTableColumnFlags_NoResize);
    ImGui::TableSetupScrollFreeze(1, 1);

    ImGuiTable* table = ImGui::GetCurrentTable();
    ImGui::TableHeadersRow();

    const ImU32 cellBg = ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_TableRowBg]);
    const ImU32 cellBgAlt = 0x25000000 //Increases alpha channel
      + ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_TableRowBgAlt]);

    ImGuiListClipper clipper(cells.size());
    while (clipper.Step())
    {
      for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
      {
        const Cellnta::NCellStorage::Vec& OrigCell = cells.OriginalAt(i);
        const Cellnta::NCellStorage::Vec& cell = cells.at(i);

        ImGui::PushID(i);

        ImGui::TableNextColumn();
        ImGui::Text("%i", i);

        ImGui::TableNextColumn();
        if (i % 2 == 0) ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cellBg);
        else ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cellBgAlt);
        Widget::TextMatrix(OrigCell.transpose());

        ImGui::TableNextColumn();
        if (i % 2 == 0) ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cellBgAlt);
        else ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cellBg);
        Widget::TextMatrix(cell.transpose());

        ImGui::PopID();
      }
    }
    ImGui::EndTable();
  }
}

void Ui::RendererWindow::PrintCamera3d(Cellnta::Camera3d& camera)
{
  ImGui::PushID(camera.GetDimensions());

  Eigen::VectorXf pos = camera.GetPosition();
  Eigen::VectorXf front = camera.GetFront();
  if (ImGui::DragFloat3("Position##CameraPos", pos.data(), 0.01f))
    camera.SetPosition(pos);

  constexpr float ItemWidth = 75.0f;
  constexpr float DragRotateSpeed = 0.002f;

  float yaw = camera.GetYawBase();
  ImGui::SetNextItemWidth(ItemWidth);
  if (ImGui::DragFloat("Yaw##Camera", &yaw, DragRotateSpeed))
    camera.SetYawBase(yaw);

  ImGui::SameLine();

  float pitch = camera.GetPitchBase();
  ImGui::SetNextItemWidth(ItemWidth);
  if (ImGui::DragFloat("Pitch##Camera", &pitch, DragRotateSpeed))
    camera.SetPitchBase(pitch);

  ImGui::Spacing();

  bool perspectiveState = camera.GetUsePerspective();
  //TODO: Dont't forget use reference
  auto viewMat = camera.GetViewMatrix();
  auto projMat = camera.GetProjectionMatrix();

  int updated = PrintCameraMatrices(&perspectiveState, nullptr, &viewMat, &projMat);
  if(updated | CameraUpdated_Projection)
    camera.SetUsePerspective(perspectiveState);
  //TODO: if used reference, just call ForceUpdateMatrix()
  if(updated | CameraUpdated_ViewMat)
    camera.SetViewMatrix(viewMat);
  if(updated | CameraUpdated_ProjMat)
    camera.SetProjectionMatrix(projMat);

  ImGui::PopID();
}

void Ui::RendererWindow::PrintCameraNd(Cellnta::CameraNd& camera)
{
  ImGui::PushID(camera.GetDimensions());

  bool needSkip = camera.NeedSkip();
  if (ImGui::Checkbox("Skip", &needSkip))
    camera.SetNeedSkip(needSkip);

  if (needSkip)
    ImGui::BeginDisabled();

  constexpr float DragSpeed = 0.01f;

  Eigen::VectorXf pos = camera.GetPosition();
  Eigen::VectorXf front = camera.GetFront();

  if(Widget::DragN("Position##CameraPos", pos.data(), pos.size(), DragSpeed))
    camera.SetPosition(pos);

  if(Widget::DragN("Target##CameraPos", front.data(), front.size(), DragSpeed))
    camera.SetFront(front);

  ImGui::Spacing();

  bool perspectiveState = camera.GetUsePerspective();
  //TODO: Dont't forget use reference
  auto upMat   = camera.GetUps();
  auto viewMat = camera.GetViewMatrix();
  auto projMat = camera.GetProjectionMatrix();

  int updated = PrintCameraMatrices(&perspectiveState, &upMat, &viewMat, &projMat);
  if(updated | CameraUpdated_Projection)
    camera.SetUsePerspective(perspectiveState);
  //TODO: if used reference, just call ForceUpdateMatrix()
  if(updated | CameraUpdated_UpMat)
    camera.SetWorldUp(upMat);
  if(updated | CameraUpdated_ViewMat)
    camera.SetViewMatrix(viewMat);
  if(updated | CameraUpdated_ProjMat)
    camera.SetProjectionMatrix(projMat);

  if (needSkip)
    ImGui::EndDisabled();

  ImGui::PopID();
}

int Ui::RendererWindow::PrintCameraMatrices(bool* perspectiveState, Eigen::MatrixXf* upMat, Eigen::MatrixXf* viewMat, Eigen::MatrixXf* projMat)
{
  constexpr const char* UpMatStr       = "Up matrix";
  constexpr const char* ViewMatStr     = "View matrix";
  constexpr const char* ProjMatStr     = "Projection matrix";
  constexpr const char* PerspectiveStr = "Current perspective";
  constexpr const char* OrthoStr       = "Current orthographic";

  int updated = CameraUpdated_None;

  if(perspectiveState != nullptr)
  {
    const char* CheckboxId = (*perspectiveState) ? PerspectiveStr : OrthoStr;

    if (ImGui::Checkbox(CheckboxId, perspectiveState))
      updated |= CameraUpdated_Projection;
  }

  if(upMat != nullptr)
  {
    if (ImGui::TreeNode(UpMatStr))
    {
      if (Widget::DragMatrix(UpMatStr, *upMat))
        updated |= CameraUpdated_UpMat;
      ImGui::TreePop();
    }
  }

  if(viewMat != nullptr)
  {
    if (ImGui::TreeNode(ViewMatStr))
    {
      if (Widget::DragMatrix(ViewMatStr, *viewMat))
        updated |= CameraUpdated_ViewMat;
      ImGui::TreePop();
    }
  }

  if(projMat != nullptr)
  {
    if (ImGui::TreeNode(ProjMatStr))
    {
      if (Widget::DragMatrix(ProjMatStr, *projMat))
        updated |= CameraUpdated_ProjMat;
      ImGui::TreePop();
    }
  }

  return updated;
}
