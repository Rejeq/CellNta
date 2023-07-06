#include <gtest/gtest.h>
#include <Cellnta/Log.h>

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  Cellnta::Log::CreateDefault()->SetLevel(Cellnta::LogBase::Level::Trace);

  return RUN_ALL_TESTS();
}

