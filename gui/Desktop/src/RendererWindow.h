#include <Renderer.h>

#include <vector>

#include "Window.h"

namespace Ui
{
  class RendererWindow : public Window
  {
  public:
    RendererWindow() : Window("Renderer") {}

    void ReadProperties(const char* line) override;
    void WriteProperties(ImGuiTextBuffer* buf) const override;
    void Draw() override;

  private:
    enum ObservedChunksColumn
    {
      CHUNK_POSITION,
      CHUNK_CELLS_QUANTITY,
    };

    enum CameraMatUpdated
    {
      CameraUpdated_None,
      CameraUpdated_Projection,
      CameraUpdated_UpMat,
      CameraUpdated_ViewMat,
      CameraUpdated_ProjMat,
    };

    void ShowCollatingInfo();
    void DrawCameras();
    void DrawLoadedCells();


    void DrawCells(const NCellStorage& cells);

    void PrintCamera3d(Camera3d& camera);
    void PrintCameraNd(CameraNd& camera);
    int PrintCameraMatrices(bool* perspectiveState, Eigen::MatrixXf* upMat, Eigen::MatrixXf* viewMat,
        Eigen::MatrixXf* projMat);
  };
}
