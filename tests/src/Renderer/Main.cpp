#include <gtest/gtest.h>
#include <Cellnta/Log.h>

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  Cellnta::Log::InitDefault();
  Cellnta::Log::GetLogger()->set_level(spdlog::level::trace);

  return RUN_ALL_TESTS();
}


