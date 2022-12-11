#pragma once

#include <vector>

#include <Cellnta/Renderer/Renderer.h>

#include "Window.h"

namespace Ui {

class RendererWindow : public Window {
  static constexpr WindowProperties DefaultProperties = {
      .Name = "Renderer",
  };

 public:
  RendererWindow() : Window(DefaultProperties) {}

  void Draw() override;

 private:
  void ShowCollatingInfo();
  void DrawLoadedCells();

  void DrawCells(const Cellnta::NCellStorage& cells);

  void DrawCameras();
  void DrawCameraSensitivity(Cellnta::Camera3d& camera);
  void DrawCamera3d(Cellnta::Camera3d& cam3d);
  void DrawCameraNdList(Cellnta::CameraNdList& list);
  void DrawCameraNd(Cellnta::CameraNd& camNd);
  void DrawCameraController(Cellnta::CameraController& controller);

  bool m_showCellsTab = false;
};

}  // namespace Ui
