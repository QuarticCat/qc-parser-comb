#pragma once

#include <array>

namespace qcpc::detail {

struct ConstCstr {
    const char* ptr;
    size_t len;

    template<size_t N>
    constexpr explicit ConstCstr(const char (&cstr)[N]) noexcept: ptr(cstr), len(N) {}
};

template<typename T>
[[nodiscard]] constexpr ConstCstr pretty_function() {
#if defined(__clang__)
    return ConstCstr(__PRETTY_FUNCTION__);
#elif defined(__GNUC__) && !defined(__clang__)
    return ConstCstr(__PRETTY_FUNCTION__);
#elif defined(_MSC_VER)
    return ConstCstr(__FUNCSIG__);
#else
    #error "No support for this compiler."
#endif
}

}  // namespace qcpc::detail
