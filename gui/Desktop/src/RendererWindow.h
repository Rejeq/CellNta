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
  enum CameraMatUpdated {
    CameraUpdated_None,
    CameraUpdated_Projection,
    CameraUpdated_UpMat,
    CameraUpdated_ViewMat,
    CameraUpdated_ProjMat,
  };

  void ShowCollatingInfo();
  void DrawCameras();
  void DrawLoadedCells();

  void DrawCells(const Cellnta::NCellStorage& cells);

  void PrintCamera3d(Cellnta::Camera3d& camera);
  void PrintCameraNd(Cellnta::CameraNd& camera);
  int PrintCameraMatrices(bool* perspectiveState, Eigen::MatrixXf* upMat,
                          Eigen::MatrixXf* viewMat, Eigen::MatrixXf* projMat);

  bool m_showCellsTab = false;
};

}  // namespace Ui
