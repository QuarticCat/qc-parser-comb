#pragma once

#include <cstddef>
#include <cstdint>

namespace qcpc::detail {

using Hash = uint64_t;

[[nodiscard]] constexpr Hash fnv_hash(const char* cstr, size_t n) {
    constexpr Hash FNV_BASIS = 14695981039346656037ull;
    constexpr Hash FNV_PRIME = 1099511628211ull;

    Hash hash = FNV_BASIS;
    for (size_t i = 0; i < n; ++i) hash = (hash ^ cstr[i]) * FNV_PRIME;
    return hash;
}

template<size_t N>
[[nodiscard]] constexpr Hash fnv_hash(const char (&cstr)[N]) {
    return fnv_hash(cstr, N - 1);
}

}  // namespace qcpc::detail
