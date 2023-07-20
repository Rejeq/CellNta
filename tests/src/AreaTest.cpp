#include <gtest/gtest.h>

#include <Cellnta/Area.h>

using namespace Cellnta;

TEST(Area, Initialization) {
  Area area;
  EXPECT_TRUE(area.Min().isZero());
  EXPECT_TRUE(area.Max().isZero());

  area = Area(Eigen::Vector3i::Zero(), Eigen::Vector3i::Zero());
  EXPECT_TRUE(area.Min().isZero());
  EXPECT_TRUE(area.Max().isZero());

  area = Area(0, 0);
  EXPECT_TRUE(area.Min().isZero());
  EXPECT_TRUE(area.Max().isZero());

  area = Area(15, 20);
  EXPECT_EQ(area.Min(), Eigen::Vector3i::Constant(15));
  EXPECT_EQ(area.Max(), Eigen::Vector3i::Constant(20));
}

TEST(Area, Valid) {
  auto CheckMinMax = [](const auto& min, const auto& max, bool exp,
                        bool invExp) {
    EXPECT_EQ(Area(min, max).Valid(), exp);
    EXPECT_EQ(Area(max, min).Valid(), invExp);
  };

  EXPECT_FALSE(Area().Valid());

  CheckMinMax(Eigen::Vector3i(20, 30, 40), Eigen::Vector3i(60, 50, 70), true,
              false);
  CheckMinMax(Eigen::Vector3i(20, 60, 40), Eigen::Vector3i(60, 50, 70), false,
              false);
}

TEST(Area, PosValid) {
  auto CheckPosValid = [](const Area& area, int val, bool minDimExp, bool exp) {
    for (int i = 1; i <= 10; ++i) {
      auto vec = Cell::Pos::Constant(i, val);
      EXPECT_EQ(area.PosValid(vec), (i <= 2) ? minDimExp : exp);
    }
  };

  Area area;
  CheckPosValid(area, 0, false, false);
  CheckPosValid(area, 10, false, false);

  area = Area(10, 100);
  CheckPosValid(area, 50, false, true);
  CheckPosValid(area, 10, false, true);
  CheckPosValid(area, 100, false, false);

  area = Area(-100, 100);
  CheckPosValid(area, 0, true, true);
  CheckPosValid(area, 50, true, true);
}

TEST(Area, Intersection) {
  auto CheckIntersection = [](const auto& l, const auto& r, const auto& exp) {
    EXPECT_EQ(Area::Intersection(l, r), exp);
    EXPECT_EQ(Area::Intersection(r, l), exp);
  };

  auto CheckValidIntersection = [](const auto& l, const auto& r, bool exp) {
    EXPECT_EQ(Area::Intersection(l, r).Valid(), exp);
    EXPECT_EQ(Area::Intersection(r, l).Valid(), exp);
  };

  CheckIntersection(Area(0, 10), Area(5, 15), Area(5, 10));
  CheckIntersection(Area(-15, -5), Area(-10, 0), Area(-10, -5));

  CheckValidIntersection(Area(0, 0), Area(0, 0), false);
  CheckValidIntersection(Area(0, 10), Area(20, 30), false);

  CheckValidIntersection(Area({0, 0, 0}, {10, 0, 0}),
                         Area({5, 0, 0}, {15, 0, 0}), false);
}

TEST(AreaTest, InverseClip_ReturnsNothingWhenSubjectBiggset) {
  const Area clipper = Area(0, 0);
  const Area subject = Area(0, 10);

  std::vector<Area> result = Area::InverseClip(clipper, subject);

  ASSERT_EQ(result.size(), 0);
}

TEST(AreaTest, InverseClip_ReturnsSubjectWhenCliipperBiggest) {
  const Area clipper = Area(0, 10);
  const Area subject = Area(0, 0);

  std::vector<Area> result = Area::InverseClip(clipper, subject);

  ASSERT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], Area(0, 10));
}

TEST(AreaTest, InverseClip_ReturnsCorrectAreas) {
  const Area clipper = Area(0, 10);
  const Area subject = Area(1, 9);

  std::vector<Area> expected = {
      Area({9, 1, 1}, {10, 10, 10}),  // Front
      Area({0, 0, 0}, {1, 9, 9}),     // Behind
      Area({1, 1, 9}, {10, 10, 10}),  // Right
      Area({0, 0, 0}, {9, 9, 1}),     // Left
      Area({1, 9, 1}, {10, 10, 10}),  // Top
      Area({0, 0, 0}, {9, 1, 9}),     // Bottom
  };

  std::vector<Area> result = Area::InverseClip(clipper, subject);

  ASSERT_EQ(result.size(), expected.size());
  for (auto& area : result) {
    auto res = std::find(expected.begin(), expected.end(), area);
    ASSERT_TRUE(res != expected.end()) << area.Min() << ", " << area.Max();
    expected.erase(res);
  }
}
