#pragma once

#include <utility>

#include "header.hpp"

namespace qcpc {

/// Match and consume any given character once.
/// `one<'a', 'b', 'c'>` means `[abc]` in PEG.
template<char... Cs>
struct One {
    QCPC_DETAIL_DEFINE_PARSE(in, ) {
        if (((*in == Cs) || ...)) {
            ++in;
            return true;
        }
        return false;
    }
};

template<char... Cs>
inline constexpr One<Cs...> one{};

/// Match and consume given string.
/// `str<'a', 'b', 'c', 'd'>` means `"abcd"` in PEG.
/// `QCPC_STR("abcd")` means `"abcd"` in PEG.
template<char... Cs>
struct Str {
    // Waiting for complete support of "Class Types in Non-Type Template Parameters" feature.
    // With this feature, we can pass a string literal as a template parameter.

    QCPC_DETAIL_DEFINE_PARSE(in, ) {
        if (in.size() < sizeof...(Cs)) return false;
        auto current = in.current();
        if (((Cs == *current++) && ...)) {
            in.advance(sizeof...(Cs));
            return true;
        }
        return false;
    }
};

// Optimization
template<char C>
struct Str<C>: One<C> {};

template<char... Cs>
inline constexpr Str<Cs...> str{};

#define QCPC_STR(s)                                \
    []<size_t... Is>(std::index_sequence<Is...>) { \
        return ::qcpc::str<(s)[Is]...>;            \
    }                                              \
    (std::make_index_sequence<sizeof(s) - 1>{})

/// Match and consume a character in given ASCII range(s).
/// `range<'a', 'z', 'A', 'Z'>` means `[a-zA-Z]` in PEG.
/// `range<'a', 'z', 'A', 'Z', '_'>` means `[a-zA-Z_]` in PEG.
template<char... Cs>
struct Range {
    QCPC_DETAIL_DEFINE_PARSE(in, ) {
        bool res = []<size_t... Is>(std::index_sequence<Is...>, char c) {
            constexpr char cs[] = {Cs...};
            constexpr size_t len = sizeof...(Cs);
            if constexpr (len % 2 == 0)
                return (check_range<cs[2 * Is], cs[2 * Is + 1]>(c) || ...);
            else
                return (check_range<cs[2 * Is], cs[2 * Is + 1]>(c) || ...) || c == cs[len - 1];
        }
        (std::make_index_sequence<sizeof...(Cs) / 2>{}, *in);

        if (res) ++in;
        return res;
    }

  private:
    template<char L, char R>
    [[nodiscard]] constexpr static bool check_range(char c) noexcept {
        static_assert(L <= R, "invalid range");
        return L <= c && c <= R;
    }
};

template<char... Cs>
inline constexpr Range<Cs...> range{};

}  // namespace qcpc
