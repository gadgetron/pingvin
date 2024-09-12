#pragma once

#include <array>
#include <iostream> /** TODO: Delete */

#include "hoNDArray.h"

namespace yardl {

namespace detail {

template <typename T>
std::vector<T> reversed(std::vector<T> const& v) {
  return std::vector<T>(v.rbegin(), v.rend());
}

template <class U>
struct initializer_depth_impl {
  static constexpr std::size_t value = 0;
};

template <class T>
struct initializer_depth_impl<std::initializer_list<T>> {
  static constexpr std::size_t value = 1 + initializer_depth_impl<T>::value;
};

template <class U>
struct initializer_dimension {
  static constexpr std::size_t value = detail::initializer_depth_impl<U>::value;
};

template <std::size_t I>
struct initializer_shape_impl {
  template <class T>
  static constexpr std::size_t value(T t) {
    return t.size() == 0 ? 0 : initializer_shape_impl<I - 1>::value(*t.begin());
  }
};

template <>
struct initializer_shape_impl<0> {
  template <class T>
  static constexpr std::size_t value(T t) {
    return t.size();
  }
};

template <class R, class U, std::size_t... I>
constexpr R initializer_shape(U t, std::index_sequence<I...>) {
  using size_type = typename R::value_type;
  return {size_type(initializer_shape_impl<I>::value(t))...};
}

template <class R, class T>
constexpr R initialize_shape(T t) {
  return detail::initializer_shape<R, decltype(t)>(t, std::make_index_sequence<detail::initializer_dimension<decltype(t)>::value>());
}

template <std::size_t X>
struct check_initializer_list_shape {
  template <class T, class S>
  static bool run(T const& t, S const& shape) {
    std::size_t IX = shape.size() - X;
    bool result = (shape[IX] == t.size());
    for (std::size_t i = 0; i < shape[IX]; ++i) {
      result = result && check_initializer_list_shape<X - 1>::run(t.begin()[i], shape);
    }
    return result;
  }
};

template <>
struct check_initializer_list_shape<0> {
  template <class T, class S>
  static bool run(T const& /*t*/, S const& /*shape*/) {
    return true;
  }
};

template <class T, std::size_t I>
struct nested_initializer_list {
  using type = std::initializer_list<typename nested_initializer_list<T, I - 1>::type>;
};

template <class T>
struct nested_initializer_list<T, 0> {
  using type = T;
};

template <class T, std::size_t I>
using nested_initializer_list_t = typename nested_initializer_list<T, I>::type;

template <class T, class S>
inline void nested_copy(T&& iter, S const& s) {
  // std::cout << "Copying " << (&s) << " to " << iter << std::endl;
  *iter++ = s;
}

template <class T, class S>
inline void nested_copy(T&& iter, std::initializer_list<S> s) {
  for (auto it = s.begin(); it != s.end(); ++it) {
    nested_copy(std::forward<T>(iter), *it);
  }
}

}  // namespace detail

/**
 * @brief A multidimensional array where all dimension sizes
 * are known at compile-time.
 *
 * @tparam T the element type
 * @tparam Dims the array dimensions
 */
// template <typename T, size_t... Dims>
// using FixedNDArray = Gadgetron::hoNDArray<T>;
template <typename T, size_t... Dims>
class FixedNDArray : public Gadgetron::hoNDArray<T> {
  using BaseType = Gadgetron::hoNDArray<T>;

 public:
  FixedNDArray()
      : BaseType() {
    auto shape = std::vector<size_t>{Dims...};
    this->create(shape);
  }

  FixedNDArray(detail::nested_initializer_list_t<T, sizeof...(Dims)> t)
      : FixedNDArray() {
    // if (!detail::check_initializer_list_shape<sizeof...(Dims)>::run(t, this->get_shape())) {
    //   throw std::runtime_error("Initializer list shape does not match FixedNDArray shape");
    // }
    auto shape = detail::initialize_shape<std::vector<size_t>>(t);
    if (shape != this->dimensions()) {
      throw std::runtime_error("Initializer list shape does not match FixedNDArray shape");
    }
    detail::nested_copy(this->begin(), t);
  }

  bool operator==(FixedNDArray const& other) const {
    return BaseType::operator==(other);
  }

  bool operator!=(FixedNDArray const& other) const {
    return BaseType::operator!=(other);
  }
};

/**
 * @brief  A multidimensional array where the number of dimensions
 * is known at compile-time
 *
 * @tparam T the element type
 * @tparam N the number of dimensions
 */
// template <typename T, size_t N>
// using NDArray = Gadgetron::hoNDArray<T>;
template <typename T, size_t N>
class NDArray : public Gadgetron::hoNDArray<T> {
  using BaseType = Gadgetron::hoNDArray<T>;
  // using Gadgetron::hoNDArray<T>::hoNDArray;

 public:
  NDArray()
      : BaseType() {}

  NDArray(BaseType const& other) : BaseType(other) {
      if (this->get_number_of_dimensions() != N) {
          throw std::runtime_error("Number of dimensions does not match");
      }
  }

  NDArray(detail::nested_initializer_list_t<T, N> t)
      : BaseType() {
    auto shape = detail::initialize_shape<std::vector<size_t>>(t);
    this->create(detail::reversed(shape));
    detail::nested_copy(this->begin(), t);
  }

  explicit NDArray(std::vector<size_t> const& shape)
      : BaseType() {
    this->create(detail::reversed(shape));
  }

  bool operator==(NDArray const& other) const {
    return BaseType::operator==(other);
  }

  bool operator!=(NDArray const& other) const {
    return BaseType::operator!=(other);
  }
};

/**
 * @brief  A multidimensional array where the number of dimensions
 * is not known at compile-time
 *
 * @tparam T the element type
 */
// template <typename T>
// using DynamicNDArray = Gadgetron::hoNDArray<T>;
template <typename T>
class DynamicNDArray : public Gadgetron::hoNDArray<T> {
  using BaseType = Gadgetron::hoNDArray<T>;

 public:
  DynamicNDArray()
      : BaseType() {}

  // template <size_t N>
  // DynamicNDArray(Gadgetron::detail::nested_initializer_list_t<T, N> t)
  //     : BaseType() {
  //   auto shape = Gadgetron::initialize_shape<std::vector<size_t>>(t);
  //   this->create(shape);
  //   Gadgetron::detail::nested_copy(this->begin(), t);
  // }
  /** TODO: There must be a way to simplify this... or perhaps not.
   * Even xtensor implements the first FIVE of these constructors...
   */
  DynamicNDArray(detail::nested_initializer_list_t<T, 1> t)
      : BaseType() {
    auto shape = detail::initialize_shape<std::vector<size_t>>(t);
    this->create(detail::reversed(shape));
    detail::nested_copy(this->begin(), t);
  }
  DynamicNDArray(detail::nested_initializer_list_t<T, 2> t)
      : BaseType() {
    auto shape = detail::initialize_shape<std::vector<size_t>>(t);
    this->create(detail::reversed(shape));
    detail::nested_copy(this->begin(), t);
  }
  DynamicNDArray(detail::nested_initializer_list_t<T, 3> t)
      : BaseType() {
    auto shape = detail::initialize_shape<std::vector<size_t>>(t);
    this->create(detail::reversed(shape));
    detail::nested_copy(this->begin(), t);
  }
  DynamicNDArray(detail::nested_initializer_list_t<T, 4> t)
      : BaseType() {
    auto shape = detail::initialize_shape<std::vector<size_t>>(t);
    this->create(detail::reversed(shape));
    detail::nested_copy(this->begin(), t);
  }

  explicit DynamicNDArray(std::vector<size_t> const& shape)
      : BaseType() {
    this->create(detail::reversed(shape));
  }

  bool operator==(DynamicNDArray const& other) const {
    return BaseType::operator==(other);
  }

  bool operator!=(DynamicNDArray const& other) const {
    return BaseType::operator!=(other);
  }
};

template <typename T>
size_t size(Gadgetron::hoNDArray<T> const& arr) {
  return arr.size();
}

template <typename T, size_t N>
std::array<size_t, N> shape(yardl::NDArray<T, N> const& arr) {
  std::array<size_t, N> ashape{};
  std::copy(arr.dimensions().rbegin(), arr.dimensions().rend(), ashape.begin());
  return ashape;
}

template <typename T>
std::vector<size_t> shape(Gadgetron::hoNDArray<T> const& arr) {
  return detail::reversed(arr.dimensions());
}

template <typename T>
size_t shape(Gadgetron::hoNDArray<T> const& arr, size_t dim) {
  // return arr.get_size(dim);
  return arr.get_size(arr.get_number_of_dimensions() - 1 - dim);
}

template <typename T>
void resize(Gadgetron::hoNDArray<T>& arr, std::vector<size_t> const& shape) {
  arr.create(detail::reversed(shape));
}

template <typename T, size_t N>
void resize(yardl::NDArray<T, N>& arr, std::array<size_t, N> shape) {
  std::vector<size_t> vshape(N);
  std::copy(shape.rbegin(), shape.rend(), vshape.begin());
  arr.create(vshape);
}

template <typename T>
size_t dimension(Gadgetron::hoNDArray<T> const& arr) {
  return arr.get_number_of_dimensions();
}

template <typename T>
T* dataptr(Gadgetron::hoNDArray<T>& arr) { return arr.data(); }
template <typename T>
T const* dataptr(Gadgetron::hoNDArray<T> const& arr) { return arr.data(); }

template <typename T, class... Args>
T const& at(Gadgetron::hoNDArray<T> const& arr, Args... idx) {
  std::vector<size_t> indices{static_cast<size_t>(idx)...};
  return arr.operator()(detail::reversed(indices));
}

}  // namespace yardl
