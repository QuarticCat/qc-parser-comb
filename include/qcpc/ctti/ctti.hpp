#pragma once

// Modified from https://github.com/Manu343726/ctti/blob/master/include/ctti/detail/hash.hpp.
// Thanks Manu343726.

#include "fnv_hash.hpp"
#include "pretty_function.hpp"

namespace qcpc {

using TypeHash = detail::Hash;

template<typename T>
[[nodiscard]] constexpr TypeHash type_hash() {
    auto cstr = detail::pretty_function<T>();
    return detail::fnv_hash(cstr.ptr, cstr.len);
}

}  // namespace qcpc
