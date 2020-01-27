#include "challenge.hpp"
#include "reference.hpp"

#include "benchmark/benchmark.h"

/***************************************************************/
// Templated Benchmark to compare both challenge and reference
template <class T> void BM_SomeFunc(benchmark::State& state)
{
    T toBeBenchmarked;
    for (auto _ : state)
    {
        toBeBenchmarked.doingSomething();
    }
    state.SetComplexityN(state.range(0)); // How many times we ran through the values
}

/***************************************************************/
#ifdef BUILD_REFERENCE
BENCHMARK_TEMPLATE(BM_SomeFunc, reference::SomeClass)->Range(1 << 0, 1 << 10)->Complexity();
#endif
BENCHMARK_TEMPLATE(BM_SomeFunc, challenge::SomeClass)->RangeMultiplier(2)->Range(1 << 0, 1 << 12)->Complexity();

// Run the benchmark
BENCHMARK_MAIN();