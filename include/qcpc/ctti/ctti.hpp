#pragma once

// Modified from https://github.com/Manu343726/ctti/blob/master/include/ctti/detail/hash.hpp.
// Thanks Manu343726.

#include "fnv_hash.hpp"
#include "pretty_function.hpp"

namespace qcpc {

using TypeHash = detail::Hash;

template<typename T>
[[nodiscard]] constexpr TypeHash type_hash() {
    auto str = detail::pretty_function<T>();
    return detail::fnv_hash(str.data(), str.size());
}

template<typename T>
[[nodiscard]] constexpr TypeHash type_hash(T) {
    return type_hash<T>();
}

}  // namespace qcpc
