#pragma once

#include <random>
#include <concepts>
#include <iterator>

constexpr const int SEED = 42;

template <typename It>
requires std::is_arithmetic_v<std::iter_value_t<It>>
void rand_init(It start, It end, std::iter_value_t<It> low, std::iter_value_t<It> up) {
  static std::mt19937_64 mt_source(SEED);

  if constexpr (std::integral<std::iter_value_t<It>>) {
    std::uniform_int_distribution<std::iter_value_t<It>> dist(low, up);
    for (auto it = start; it != end; ++it) {
      *it = dist(mt_source);
    }
  } else {
    std::uniform_real_distribution<std::iter_value_t<It>> dist(low, up);
    for (auto it = start; it != end; ++it) {
      *it = dist(mt_source);
    }
  }
}

inline size_t largest_divisor_leq_limit(size_t dividend, size_t limiter) {
  for (size_t d = limiter; d > 0; --d) {
    if (dividend % d == 0) { return d; }
  }
  return 1;
}

inline bool is_power_of_2(size_t x) {
  return (x > 0) && ((x & (x - 1)) == 0);
}