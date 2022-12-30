#include "RendererWindow.h"

#include <array>

#include <Cellnta/Renderer/Camera3d.h>
#include <Cellnta/Renderer/CameraNd.h>
#include <Cellnta/Renderer/HypercubeStorage.h>
#include <Cellnta/Renderer/RenderData.h>
#include <Cellnta/Renderer/Renderer.h>

#include "Context.h"
#include "Widgets/ComboEnum.h"
#include "Widgets/Eigen.h"
#include "Widgets/Utils.h"

using namespace Ui;

void RendererWindow::Draw() {
  CELLNTA_PROFILE;

  constexpr ImGuiWindowFlags WinFlags = ImGuiWindowFlags_HorizontalScrollbar;

  Cellnta::Renderer& ren = GetContext()->GetRenderer();
  Cellnta::RenderData* data = ren.GetData();
  Cellnta::HypercubeStorage* cube = ren.GetHypercube();

  if (data == nullptr || cube == nullptr)
    return;

  if (ImGui::Begin(p_prop.Name, &p_prop.Opened, WinFlags)) {
    uint32_t dimensions = ren.GetDimensions();
    if (Widget::Input("Cube dimensions", &dimensions, 1,
                      ImGuiInputTextFlags_CharsDecimal)) {
      ren.SetDimension(dimensions);
    }

    uint32_t renderDistance = data->GetDistance();
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
    if (Widget::ComboEnum("Cube mode", cube->GetMode(), CubeModeData, res))
      cube->SetMode(res);

    Widget::Separator();

    Widget::CellSelectorHomogeneous(ren.GetDimensions(), m_selectedCell);

<<<<<<< Updated upstream
||||||| constructed merge base
    static Cellnta::Cell cell;
    Widget::CellSelector<true>(ren.GetDimensions(), cell);
=======
    static Cellnta::Cell cell;
    Widget::CellSelectorHomogeneous(ren.GetDimensions(), cell);
>>>>>>> Stashed changes
    ImGui::Spacing();

    if (ImGui::Button("Add cell (Only for renderer)"))
      data->SetCell(m_selectedCell);

    Widget::Separator();

    ShowCollatingInfo();

    ImGui::Spacing();

    const size_t cellsSize = data->GetCells().size();

    ImGuiTreeNodeFlags nodeFlags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;
    if (ImGui::TreeNodeEx("Loaded cells", nodeFlags, "Loaded cells (%zu)",
                          cellsSize)) {
      ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
      DrawLoadedCells();
      ImGui::TreePop();
    }
  }

  ImGui::End();
}

void RendererWindow::ShowCollatingInfo() {
  Cellnta::Renderer& ren = GetContext()->GetRenderer();
  Cellnta::RenderData* data = ren.GetData();
  if (data == nullptr)
    return;

  int x = data->GetCollatingX();
  int y = data->GetCollatingY();
  int z = data->GetCollatingZ();
  if (ImGui::InputInt("X", &x))
    data->SetCollatingX(x);
  if (ImGui::InputInt("Y", &y))
    data->SetCollatingY(y);
  if (ImGui::InputInt("Z", &z))
    data->SetCollatingZ(z);
}

void RendererWindow::DrawLoadedCells() {
  CELLNTA_PROFILE;

  Cellnta::Renderer& ren = GetContext()->GetRenderer();
  Cellnta::RenderData* data = ren.GetData();
  if (data == nullptr)
    return;

  const Cellnta::NCellStorage& cells = data->GetCells();
  if (cells.size() == 0)
    return;

  constexpr ImGuiTableFlags TableFlags =
      ImGuiTableFlags_Borders | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY |
      ImGuiTableFlags_Hideable;

  const ImVec2 outerSize = ImVec2(0, ImGui::GetFontSize() * 15);
  if (ImGui::BeginTable("tableCells", 3, TableFlags, outerSize, 0.0f)) {
    ImGui::TableSetupColumn(
        "#", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_NoHide);
    ImGui::TableSetupColumn("Original", ImGuiTableColumnFlags_NoResize);
    ImGui::TableSetupColumn("Visible", ImGuiTableColumnFlags_NoResize);
    ImGui::TableSetupScrollFreeze(1, 1);

    const auto& rawCells = cells.GetRaw();
    const auto& rawVisibleCells = cells.GetVisibleRaw();
    ImGuiTable* table = ImGui::GetCurrentTable();
    ImGui::TableHeadersRow();

    const ImU32 cellBg =
        ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_TableRowBg]);
    const ImU32 cellBgAlt =
        0x25000000  // Increases alpha channel
        + ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_TableRowBgAlt]);

    ImGuiListClipper clipper;

    clipper.Begin(cells.size());
    while (clipper.Step()) {
      for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
        ImGui::PushID(i);

        ImGui::TableNextColumn();
        ImGui::Text("%i", i);

        ImGui::TableNextColumn();
        if (i % 2 == 0)
          ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cellBg);
        else
          ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cellBgAlt);

        const auto& cell = rawCells.at(i);
        Widget::TextMatrix(cell.transpose());

        ImGui::TableNextColumn();
        if (i % 2 == 0)
          ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cellBgAlt);
        else
          ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cellBg);

        const auto& visibleCell = rawVisibleCells.at(i);
        Widget::TextMatrix(visibleCell.transpose());

        ImGui::PopID();
      }
    }
    clipper.End();
    ImGui::EndTable();
  }
}
