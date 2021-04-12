#pragma once

#include "ascii.hpp"
#include "combinator.hpp"
#include "zero_width.hpp"

namespace qcpc {

// Compound Rules

// TODO: docs and tests of these rules

inline constexpr auto ident_first = range<'a', 'z', 'A', 'Z', '_'>;

inline constexpr auto ident_other = range<'a', 'z', 'A', 'Z', '0', '9', '_'>;

inline constexpr auto ident = ident_first & *ident_other;

template<char... Cs>
inline constexpr auto keyword = str<Cs...> & !ident_other;

#define QCPC_KEYWORD(s)                            \
    []<size_t... Is>(std::index_sequence<Is...>) { \
        return ::qcpc::keyword<(s)[Is]...>;        \
    }                                              \
    (std::make_index_sequence<sizeof(s) - 1>{})

}  // namespace qcpc
