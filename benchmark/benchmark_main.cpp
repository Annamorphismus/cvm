#include <benchmark/benchmark.h>

#include <cstddef>
#include <cstdint>
#include <cvm/cvm_knuth.hpp>
#include <cvm/cvm_naive.hpp>
#include <iostream>
#include <random>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

template <class T>
[[nodiscard]] inline auto random_vec(std::size_t N) noexcept -> std::vector<T> {
  static_assert(std::is_integral_v<T>, "T should be an integral type");

  std::vector<T> arr(N);
  auto gen = []() constexpr {
    if constexpr (std::is_same_v<T, std::uint64_t>) {
      return std::mt19937_64(std::random_device{}());
    } else {
      return std::mt19937(std::random_device{}());
    }
  }();

  std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(),
                                        std::numeric_limits<T>::max());

  for (auto &elem : arr) {
    elem = dist(gen);
  }

  return arr;
}

template <class T> inline static auto naive(benchmark::State &state) {
  auto N = static_cast<double>(state.range(0));
  auto i = static_cast<double>(state.range(1));
  auto j = static_cast<std::size_t>(state.range(2));

  double eps = static_cast<double>(i) / 10.;
  double delta = static_cast<double>(j) * 0.0001;

  for (auto _ : state) {
    state.PauseTiming();
    auto vec = random_vec<T>(N);
    state.ResumeTiming();

    auto result = cvm::naive_cvm(std::begin(vec), std::end(vec), eps, delta);

    benchmark::DoNotOptimize(result);
  }
}

template <class T> inline static auto knuth(benchmark::State &state) {
  auto N = static_cast<double>(state.range(0));
  auto s = static_cast<double>(state.range(1));

  for (auto _ : state) {
    state.PauseTiming();
    auto vec = random_vec<T>(N);
    state.ResumeTiming();

    auto result = cvm::knuth_cvm(std::begin(vec), std::end(vec), s);

    benchmark::DoNotOptimize(result);
  }
}

static void CustomArgumentsNaive(benchmark::internal::Benchmark *b) {
  for (std::int64_t N = 1; N <= 10000000; N *= 10) {
    for (std::int64_t i = 1; i <= 10; i += 2) {
      for (std::int64_t j = 1; j <= 1000; j *= 10) {
        double epsilon = static_cast<double>(i) / 10.;
        double delta = static_cast<double>(j) * 0.0001;
        std::int64_t thresh = (12. / epsilon * epsilon) *
                              std::log2((8. * static_cast<double>(N)) / delta);

        if (thresh < N) {
          b->Args({N, i, j});
        }
      }
    }
  }
}

static void CustomArgumentsKnuth(benchmark::internal::Benchmark *b) {
  for (std::int64_t N = 1; N <= 10000000; N *= 10) {
    for (std::int64_t s = 1; s <= N; s *= 10) {
      b->Args({N, s});
    }
  }
}

// BENCHMARK(knuth<std::uint8_t>)->Apply(CustomArgumentsKnuth);
// BENCHMARK(knuth<std::uint16_t>)->Apply(CustomArgumentsKnuth);
// BENCHMARK(knuth<std::uint32_t>)->Apply(CustomArgumentsKnuth);
// BENCHMARK(knuth<std::uint64_t>)->Apply(CustomArgumentsKnuth);

BENCHMARK(naive<std::uint8_t>)->Apply(CustomArgumentsNaive);
BENCHMARK(naive<std::uint16_t>)->Apply(CustomArgumentsNaive);
BENCHMARK(naive<std::uint32_t>)->Apply(CustomArgumentsNaive);
BENCHMARK(naive<std::uint64_t>)->Apply(CustomArgumentsNaive);

// main function
BENCHMARK_MAIN();
