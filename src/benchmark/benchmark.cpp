#include "benchmark/benchmark.h"

/***************************************************************/
// Templated Benchmark to compare both challenge and reference
template <class T> void BM_SomeFunc(benchmark::State& state)
{
    T toBeBenchmarked = 0;
    for (auto _ : state)
    {
        toBeBenchmarked += 10;
    }
    state.SetComplexityN(state.range(0)); // How many times we ran through the values
}

/***************************************************************/
BENCHMARK_TEMPLATE(BM_SomeFunc, int)->RangeMultiplier(2)->Range(1 << 0, 1 << 12)->Complexity();

// Run the benchmark
BENCHMARK_MAIN();