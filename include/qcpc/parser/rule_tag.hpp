#pragma once

#include <string_view>

namespace qcpc {

using RuleTag = size_t;

inline constexpr RuleTag NO_RULE = std::numeric_limits<RuleTag>::max();

namespace detail {

template<typename T>  // Can not remove `T` here
consteval RuleTag rule_tag() {
    // Get a string that contains type name of T from a special variable / macro
#if defined(__GNUC__) || defined(__clang__)
    std::string_view name = __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
    std::string_view name = __FUNCSIG__;
#else
    #error "No support for this compiler."
#endif

    // FNV-1a hash
    constexpr RuleTag FNV_BASIS = 14695981039346656037ull;
    constexpr RuleTag FNV_PRIME = 1099511628211ull;
    RuleTag hash = FNV_BASIS;
    for (char c: name) hash = (hash ^ c) * FNV_PRIME;
    return hash;
}

}  // namespace detail

}  // namespace qcpc
