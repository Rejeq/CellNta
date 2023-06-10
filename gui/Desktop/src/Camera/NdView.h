#pragma once

#include <Cellnta/Renderer/CameraNd.h>

#include "Context.h"

namespace Ui {

class CameraNdView {
 public:
  static void Draw(Context& ctx, Cellnta::CameraNdList& camList);
  static void DrawInstance(Context& ctx, Cellnta::CameraNdList& camList,
                           int idx);

 private:
  static void DrawInstance(Context& ctx, Cellnta::CameraNd& cam, int idx);
};

}  // namespace Ui
