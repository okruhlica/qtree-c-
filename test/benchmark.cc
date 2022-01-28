#include "../src/qtree.h"
#include <benchmark/benchmark.h>

float random_float(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}
static void BM_FillQuadTree1M(benchmark::State& state){
    for (auto _ : state) {
        auto tree = AdamLib::QuadTree({0, 0, 100, 100}, 1000 * 1000);
        for (int i = 1; i <= 1000 * 1000; ++i) {
            tree.insert({random_float(0,100), random_float(0,100)});
        }
    }
}

static void BM_FillQuadTree1MNoInit(benchmark::State& state){
    for (auto _ : state) {
        auto tree = AdamLib::QuadTree({0, 0, 100, 100});
        for (int i = 1; i <= 1000 * 1000; ++i) {
            tree.insert({random_float(0,100), random_float(0,100)});
        }
    }
}

static void BM_InitQuadTree1M(benchmark::State& state){
    for (auto _ : state) {
        auto tree =  AdamLib::QuadTree({0, 0, 100, 100}, 1000 * 1000);
    }
}


static void BM_InitQuadTreeNoInit(benchmark::State& state){
    for (auto _ : state) {
        auto tree = AdamLib::QuadTree({0, 0, 100, 100});
    }
}

// Register the function as a benchmark
BENCHMARK(BM_InitQuadTreeNoInit)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_InitQuadTree1M)->Unit(benchmark::kMillisecond);

BENCHMARK(BM_FillQuadTree1MNoInit)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_FillQuadTree1M)->Unit(benchmark::kMillisecond);;

BENCHMARK_MAIN();