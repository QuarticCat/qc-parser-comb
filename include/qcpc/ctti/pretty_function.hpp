#pragma once

#include <array>
#include <string_view>

namespace qcpc::detail {

template<typename T>  // `T` is necessary here for showing typename on `__PRETTY_FUNCTION__`
[[nodiscard]] constexpr std::string_view pretty_function() {
#if defined(__clang__)
    return __PRETTY_FUNCTION__;
#elif defined(__GNUC__) && !defined(__clang__)
    return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
    return __FUNCSIG__;
#else
    #error "No support for this compiler."
#endif
}

}  // namespace qcpc::detail
