#pragma once

#include "Window.h"

#include <Cellnta/Renderer/Camera3d.h>
#include <Cellnta/Renderer/CameraNd.h>

namespace Ui {

class CameraWindow : public Window {
  static constexpr WindowProperties DefaultProperties = {
      .Name = "Camera",
  };

 public:
  CameraWindow() : Window(DefaultProperties) {}

  void Draw() override;

 private:
  void DrawCameras();
  void DrawCameraOptions(Cellnta::Camera3d& camera);
  void DrawCamera3d(Cellnta::Camera3d& cam3d);
  void DrawCameraNdList(Cellnta::CameraNdList& list);
  void DrawCameraNd(Cellnta::CameraNd& camNd);
  void DrawCameraController(Cellnta::CameraController& controller);
};

}  // namespace Ui
