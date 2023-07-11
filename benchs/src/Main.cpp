#include <benchmark/benchmark.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <Cellnta/Log.h>

std::string GetFileNameFromPath(const std::string& path) {
  return path.substr(path.find_last_of('\\') + 1);
}

int main(int argc, char** argv) {
  std::string programName;
  if (argc > 0)
    programName = GetFileNameFromPath(argv[0]);

  Cellnta::Log::Create();
  Cellnta::Log::Get().SetLevel(Cellnta::LogBase::Level::Info);
  Cellnta::Log::Get().SetDefaultPattern();

  // Output to a file because otherwise benchmark output table will be broken
  const spdlog::filename_t logFile = "BenchmarkLogsFor_" + programName + ".txt";
  Cellnta::Log::Get().AddSink<spdlog::sinks::basic_file_sink_mt>(logFile, true);

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
}
