#include "Renderer/RenderDataView.h"

#include "Renderer/RenderDataAction.h"
#include "Widgets/Eigen.h"
#include "Widgets/Utils.h"

using namespace Ui;

void RenderDataView::Draw(Context& ctx, Cellnta::RenderData& data) {
  CELLNTA_PROFILE;

  uint32_t renderDistance = data.GetDistance();
  // TODO: from keyboard input negative number not clamped
  if (Widget::Input("Render distance", &renderDistance, 1,
                    ImGuiInputTextFlags_CharsDecimal))
    ctx.PushAction(
        Action::Make(Action::Renderer::SetRenderDistance(renderDistance)));

  Widget::CellSelectorHomogeneous(data.GetDimension(), m_selectedCell);

  ImGui::Spacing();

  if (ImGui::Button("Add cell (Only for renderer)"))
    ctx.PushAction(Action::Make(Action::Renderer::SetCell(m_selectedCell)));

  ImGui::Spacing();

  const auto& cells = data.GetCells();
  const size_t cellsSize = cells.Size();

  ImGuiTreeNodeFlags nodeFlags =
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
      ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;
  if (ImGui::TreeNodeEx("Loaded cells", nodeFlags, "Loaded cells (%zu)",
                        cellsSize)) {
    ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

    DrawLoadedCells(cells);

    ImGui::TreePop();
  }
}

void RenderDataView::DrawLoadedCells(const Cellnta::NCellStorage& cells) {
  CELLNTA_PROFILE;

  if (cells.Size() == 0)
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

    auto rawCells = cells.GetRaw().MakeWholeIter();
    auto rawVisibleCells = cells.GetVisibleRaw().MakeWholeIter();
    ImGui::TableHeadersRow();

    const ImU32 cellBg =
        ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_TableRowBg]);
    const ImU32 cellBgAlt =
        0x25000000  // Increases alpha channel
        + ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_TableRowBgAlt]);

    ImGuiListClipper clipper;

    clipper.Begin(cells.Size());
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

        const auto* cell = rawCells.Next();
        Widget::TextMatrix(cell->pos.transpose());

        ImGui::TableNextColumn();
        if (i % 2 == 0)
          ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cellBgAlt);
        else
          ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cellBg);

        const auto* visibleCell = rawVisibleCells.Next();
        Widget::TextMatrix(visibleCell->pos.transpose());

        ImGui::PopID();
      }
    }
    clipper.End();
  }
}
