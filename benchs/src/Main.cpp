#include <benchmark/benchmark.h>
#include <Cellnta/Log.h>

int main(int argc, char** argv) {
  Cellnta::Log::InitDefault();
  Cellnta::Log::GetLogger()->set_level(spdlog::level::info);

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
}

