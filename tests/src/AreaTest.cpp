#include <gtest/gtest.h>

#include <Cellnta/Area.h>

using namespace Cellnta;

TEST(Cell, Initialization) {
  Cell cell;
  EXPECT_TRUE(cell.pos.isZero());
  EXPECT_EQ(cell.state, 0);

  cell = Cell(Eigen::Vector3i::Zero(), 25);
  EXPECT_TRUE(cell.pos.isZero());
  EXPECT_EQ(cell.state, 25);

  cell = Cell(Eigen::Vector3i::Constant(10), 25);
  EXPECT_EQ(cell.pos, Eigen::Vector3i::Constant(10));
  EXPECT_EQ(cell.state, 25);
}

TEST(Area, Initialization) {
  Area area;
  EXPECT_TRUE(area.min.isZero());
  EXPECT_TRUE(area.max.isZero());

  area = Area(Eigen::Vector3i::Zero(), Eigen::Vector3i::Zero());
  EXPECT_TRUE(area.min.isZero());
  EXPECT_TRUE(area.max.isZero());

  area = Area(0, 0);
  EXPECT_TRUE(area.min.isZero());
  EXPECT_TRUE(area.max.isZero());

  area = Area(15, 20);
  EXPECT_EQ(area.min, Eigen::Vector3i::Constant(15));
  EXPECT_EQ(area.max, Eigen::Vector3i::Constant(20));
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
  CheckPosValid(area, 10, false, false);
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

  CheckValidIntersection(
      Area({0, 0, 0}, {10, 0, 0}), Area({5, 0, 0}, {15, 0, 0}), false);
}

TEST(AreaTest, InverseClip) {
  Area expected;
  Area clipper;
  Area subject;
  std::vector<Area> result;

  clipper = Area(0, 0);
  subject = Area(0, 10);
  result = Area::InverseClip(clipper, subject);
  ASSERT_EQ(result.size(), 0);

  result = Area::InverseClip(subject, clipper);
  ASSERT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], Area(0, 10));

  // FIXME: For this case it gives wrong results - 4 corners are not handled
  // Its easy to fix, but I'm not sure that fixing it won't break other
  // conditions
  clipper = Area(0, 10);
  subject = Area(1, 9);
  result = Area::InverseClip(clipper, subject);
  ASSERT_EQ(result.size(), 6);

  Area front = Area({9, 1, 1}, {10, 10, 10});
  EXPECT_EQ(result[0], front) << result[0].min << ", " << result[0].max;

  // FIXME: Must be Area({0, 0, 0}, {1, 10, 10})
  Area behind = Area({0, 0, 0}, {1, 9, 9});
  EXPECT_EQ(result[1], behind) << result[1].min << ", " << result[1].max;

  Area right = Area({1, 1, 9}, {10, 10, 10});
  EXPECT_EQ(result[2], right) << result[2].min << ", " << result[2].max;

  Area left = Area({0, 0, 0}, {9, 1, 9});
  EXPECT_EQ(result[3], left) << result[3].min << ", " << result[3].max;

  Area top = Area({1, 9, 1}, {10, 10, 10});
  EXPECT_EQ(result[4], top)<< result[4].min << ", " << result[4].max;

  Area bottom = Area({0, 0, 0}, {9, 9, 1});
  EXPECT_EQ(result[5], bottom)<< result[5].min << ", " << result[5].max;
}

