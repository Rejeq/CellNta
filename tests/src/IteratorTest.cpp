#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Cellnta/Area.h>
#include <Cellnta/Iterator.h>

using namespace Cellnta;

class MockIterator : public Iterator {
 public:
  MOCK_METHOD((void), Reset, (), ());
  MOCK_METHOD((const Cell*), Next, (), ());
};

TEST(Iterator, Nth) {
  constexpr int count = 5;
  Cell cell = Cell(Cell::Pos::Constant(1, 1), 1);

  MockIterator iter;
  EXPECT_CALL(iter, Next())
      .Times(count)
      .WillRepeatedly(::testing::Return(&cell));

  iter.Nth(count);
}

TEST(Iterator, EmptyNth) {
  MockIterator iter;
  EXPECT_CALL(iter, Next()).Times(1);
  iter.Nth(123);
}

TEST(Iterator, NegativeNth) {
  MockIterator iter;
  EXPECT_CALL(iter, Next()).Times(0);
  iter.Nth(-123);
}
