namespace iss::ocl {

template <typename T>
struct ocl_type_name;

template<>
struct ocl_type_name<float> {
  static constexpr const char* value = "float";
};

template<>
struct ocl_type_name<int> {
  static constexpr const char* value = "int";
};

template<>
struct ocl_type_name<double> {
  static constexpr const char* value = "double";
};

} // namespace iss::ocl