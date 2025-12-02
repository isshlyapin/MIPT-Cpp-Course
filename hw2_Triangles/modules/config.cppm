export module isshlyapin.config;

export template <typename T>
constexpr T EPS = T{1e-9}; // значение по умолчанию

export template <>
constexpr float EPS<float> = 1e-5f;

export template <>
constexpr double EPS<double> = 1e-9;

export template <>
constexpr long double EPS<long double> = 1e-12L;