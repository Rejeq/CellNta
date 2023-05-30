#include <gtest/gtest.h>

#include <Cellnta/Renderer/HypercubeStorage.h>

using namespace Cellnta;

TEST(HypercubeStorage, PolygonMode_UniqueFaces) {
  HypercubeStorage cube;
  cube.GenerateCube(8, -1.0f, CubeMode::POLYGON);

  int size = cube.GetIndicesSize();
  const auto* ind = cube.GetIndices();

  for (int i = 0; i < size; i += 6) {
    for (int j = 0; j < i; j += 6) {
      ASSERT_FALSE(ind[i + 0] == ind[j + 0] && ind[i + 1] == ind[j + 1] &&
                   ind[i + 2] == ind[j + 2] && ind[i + 3] == ind[j + 3] &&
                   ind[i + 4] == ind[j + 4] && ind[i + 5] == ind[j + 5])
          << "i = '" << i << "', j = '" << j << "'";
    }
  }
}
