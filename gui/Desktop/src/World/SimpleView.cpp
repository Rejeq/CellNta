#include "World/SimpleView.h"

#include <Cellnta/World/Impl/Simple.h>

#include "World/SimpleAction.h"
#include "Widgets/Utils.h"

using namespace Ui;

void WorldSimpleView::Draw(Context& ctx, Cellnta::WorldImplSimple& world) {
  CELLNTA_PROFILE;

  bool repeated = world.GetWorldRepeated();
  if (ImGui::Checkbox("World repeated", &repeated))
    ctx.PushAction(Action::Make(Action::SimpleWorld::SetRepeated(repeated)));

  auto size = world.GetSize();
  if (Widget::DragN("Size", size.data(), size.size()))
    ctx.PushAction(Action::Make(Action::SimpleWorld::SetSize(size)));

  Widget::Separator();

  boost::dynamic_bitset<> born = world.GetBorn();
  if (Widget::RuleMask("Born", born))
    ctx.PushAction(Action::Make(Action::SimpleWorld::SetBorn(born)));

  boost::dynamic_bitset<> survive = world.GetSurvive();
  if (Widget::RuleMask("Survive", survive))
    ctx.PushAction(Action::Make(Action::SimpleWorld::SetSurvive(survive)));
}
