#include "Cellnta/Renderer/CameraController.h"

using namespace Cellnta;

bool CameraController::Update() {
  bool out = p_updateMatrix;
  p_updateMatrix = false;

  if (p_updateView) {
    UpdateViewMatrix();
    p_updateView = false;
    out = true;
  }
  if (p_updateProj) {
    UpdateProjMatrix();
    p_updateProj = false;
    out = true;
  }

  return out;
}

void CameraController::SetUsePerspective(bool newState) {
  p_usePerspective = newState;
  p_updateMatrix = true;
}
