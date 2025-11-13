#include <map>
#include <vector>
#include <random>
#include <iterator>
#include <algorithm>

#include <benchmark/benchmark.h>

#include "tree.hpp"

const int SEED = 42;

// ======================================================
// 1️⃣ Benchmark: std::map — вставка
// ======================================================
static void BM_MapInsert(benchmark::State& state) {
    const int N = static_cast<int>(state.range(0));
    std::mt19937 rng(SEED);
    std::uniform_int_distribution<int> dist(0, N * 10);

    for (auto _ : state) {
        state.PauseTiming();
        std::vector<int> data(N);
        for (int& x : data) x = dist(rng);
        state.ResumeTiming();

        std::map<int, int> m;
        for (int i = 0; i < N; ++i)
            m.emplace(data[i], i);

        benchmark::DoNotOptimize(m);
    }

    state.SetItemsProcessed(int64_t(state.iterations()) * N);
}

BENCHMARK(BM_MapInsert)->Arg(10'000)->Arg(100'000)->Arg(1'000'000);


// ======================================================
// 2️⃣ Benchmark: std::map — lower_bound + upper_bound + distance
// ======================================================
static void BM_MapRangeQuery(benchmark::State& state) {
    const int N = static_cast<int>(state.range(0));
    const int Q = 100;

    std::mt19937 rng(SEED);
    std::uniform_int_distribution<int> dist(0, N * 10);

    std::map<int, int> m;
    for (int i = 0; i < N; ++i)
        m.emplace(dist(rng), i);

    std::vector<std::pair<int, int>> queries(Q);
    for (auto& [a, b] : queries) {
        a = dist(rng);
        b = dist(rng);
        if (a > b) std::swap(a, b);
    }

    for (auto _ : state) {
        size_t total = 0;
        for (const auto& [a, b] : queries) {
            auto it_low = m.lower_bound(a);
            auto it_up  = m.upper_bound(b);
            total += std::distance(it_low, it_up);
        }
        benchmark::DoNotOptimize(total);
    }

    state.SetItemsProcessed(int64_t(state.iterations()) * Q);
}

BENCHMARK(BM_MapRangeQuery)->Arg(10'000)->Arg(100'000)->Arg(1'000'000);


// ======================================================
// 3️⃣ Benchmark: ThreadedBinaryTree — вставка
// ======================================================
static void BM_TreeInsert(benchmark::State& state) {
    const int N = static_cast<int>(state.range(0));
    std::mt19937 rng(SEED);
    std::uniform_int_distribution<int> dist(0, N * 10);

    for (auto _ : state) {
        state.PauseTiming();
        std::vector<int> data(N);
        for (int& x : data) x = dist(rng);
        state.ResumeTiming();

        myds::ThreadedBinaryTree<int, int> tree;
        for (int i = 0; i < N; ++i)
            tree.insert(data[i], i);

        benchmark::DoNotOptimize(tree);
    }

    state.SetItemsProcessed(int64_t(state.iterations()) * N);
}

BENCHMARK(BM_TreeInsert)->Arg(10'000)->Arg(100'000)->Arg(1'000'000);


// ======================================================
// 4️⃣ Benchmark: ThreadedBinaryTree — lower_bound + upper_bound + distance
// ======================================================
static void BM_TreeRangeQuery(benchmark::State& state) {
    const int N = static_cast<int>(state.range(0));
    const int Q = 100;

    std::mt19937 rng(SEED);
    std::uniform_int_distribution<int> dist(0, N * 10);

    myds::ThreadedBinaryTree<int, int> tree;
    for (int i = 0; i < N; ++i)
        tree.insert(dist(rng), i);

    std::vector<std::pair<int, int>> queries(Q);
    for (auto& [a, b] : queries) {
        a = dist(rng);
        b = dist(rng);
        if (a > b) std::swap(a, b);
    }

    for (auto _ : state) {
        size_t total = 0;
        for (const auto& [a, b] : queries) {
            auto it_low = tree.lower_bound(a);
            auto it_up  = tree.upper_bound(b);
            total += std::distance(it_low, it_up);
        }
        benchmark::DoNotOptimize(total);
    }

    state.SetItemsProcessed(int64_t(state.iterations()) * Q);
}

BENCHMARK(BM_TreeRangeQuery)->Arg(10'000)->Arg(100'000)->Arg(1'000'000);


// ======================================================
BENCHMARK_MAIN();
