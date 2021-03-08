#pragma once

#include <algorithm>
#include <concepts>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

#include "../input/input.hpp"
#include "rule_tag.hpp"
#include "token.hpp"

namespace qcpc {

/// Concept to check if a type is a rule type.
// clang-format off
template<class T>
concept RuleType = std::is_empty_v<T> && requires(MemoryInput& in, Token::Children& out) {
    { T::parse(in, out) } -> std::same_as<bool>;
};
// clang-format on

// This macro is mainly for reducing refactoring workload during early development.
#define DEFINE_PARSE(in, out) \
    template<InputType Input> \
    static bool parse(Input& in, Token::Children& out) noexcept

/// Match the beginning of input. Consume nothing.
struct Boi {
    DEFINE_PARSE(in, ) {
        return in.is_boi();
    }
};

inline constexpr Boi boi{};

/// Match the end of input. Consume nothing.
struct Eoi {
    DEFINE_PARSE(in, ) {
        return in.is_eoi();
    }
};

inline constexpr Eoi eoi{};

/// Match the beginning of lines. Consume nothing.
struct Bol {
    DEFINE_PARSE(in, ) {
        return in.column() == 0;
    }
};

inline constexpr Bol bol{};

/// Match the end of lines. Consume "\r\n" or "\n".
struct Eol {
    DEFINE_PARSE(in, ) {
        auto pos = in.pos();
        if (*in == '\n') {
            ++in;
            return true;
        }
        if (*in == '\r') {
            if (*++in == '\n') {
                ++in;
                return true;
            }
            in.jump(pos);
        }
        return false;
    }
};

inline constexpr Eol eol{};

/// Match and consume any given character once.
/// `one<'a', 'b', 'c'>` means `[abc]` in PEG.
template<char... Cs>
struct One {
    DEFINE_PARSE(in, ) {
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

    DEFINE_PARSE(in, ) {
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

#define QCPC_STR(str)                              \
    []<size_t... Is>(std::index_sequence<Is...>) { \
        return ::qcpc::Str<str[Is]...>{};          \
    }                                              \
    (std::make_index_sequence<sizeof(str) - 1>{})

/// Match and consume a character in given ASCII range(s).
/// `range<'a', 'z', 'A', 'Z'>` means `[a-zA-Z]` in PEG.
template<char... Cs>
struct Range {
    static_assert(sizeof...(Cs) % 2 == 0, "Param number should be even.");
    // TODO: Should we check the validity of ranges?

    DEFINE_PARSE(in, ) {
        constexpr char cs[] = {Cs...};  // by tests, compiler will inline this
        for (size_t i = 0; i < sizeof...(Cs); i += 2) {
            if (cs[i] <= *in && *in <= cs[i + 1]) {
                ++in;
                return true;
            }
        }
        return false;
    }
};

template<char... Cs>
inline constexpr Range<Cs...> range{};

/// PEG and-predicate `&e`.
template<RuleType R>
struct At {
    DEFINE_PARSE(in, out) {
        auto pos = in.pos();
        auto ret = R::parse(in, out);
        in.jump(pos);
        return ret;
    }
};

template<RuleType R>
[[nodiscard]] constexpr At<R> operator&(R) {
    return {};
}

/// PEG not-predicate `!e`.
template<RuleType R>
struct NotAt {
    DEFINE_PARSE(in, out) {
        auto pos = in.pos();
        auto ret = R::parse(in, out);
        in.jump(pos);
        return !ret;
    }
};

template<RuleType R>
[[nodiscard]] constexpr NotAt<R> operator!(R) {
    return {};
}

/// PEG optional `e?`.
template<RuleType R>
struct Opt {
    DEFINE_PARSE(in, out) {
        R::parse(in, out);
        return true;
    }
};

template<RuleType R>
[[nodiscard]] constexpr Opt<R> operator-(R) {
    return {};
}

/// PEG zero-or-more `e*`.
template<RuleType R>
struct Star {
    DEFINE_PARSE(in, out) {
        while (R::parse(in, out)) {}
        return true;
    }
};

template<RuleType R>
[[nodiscard]] constexpr Star<R> operator*(R) {
    return {};
}

/// PEG one-or-more `e+`.
template<RuleType R>
struct Plus {
    DEFINE_PARSE(in, out) {
        if (!R::parse(in, out)) return false;
        while (R::parse(in, out)) {}
        return true;
    }
};

template<RuleType R>
[[nodiscard]] constexpr Plus<R> operator+(R) {
    return {};
}

/// PEG sequence `e1 e2`.
template<RuleType... Rs>
struct Seq {
    DEFINE_PARSE(in, out) {
        auto pos = in.pos();
        size_t size = out.size();
        if ((Rs::parse(in, out) && ...)) return true;
        in.jump(pos);
        out.erase(out.begin() + size, out.end());
        return false;
    }
};

template<RuleType R1, RuleType R2>
[[nodiscard]] constexpr Seq<R1, R2> operator&(R1, R2) {
    return {};
}

template<RuleType R1, RuleType... R2s>
[[nodiscard]] constexpr Seq<R1, R2s...> operator&(R1, Seq<R2s...>) {
    return {};
}

template<RuleType... R1s, RuleType R2>
[[nodiscard]] constexpr Seq<R1s..., R2> operator&(Seq<R1s...>, R2) {
    return {};
}

template<RuleType... R1s, RuleType... R2s>
[[nodiscard]] constexpr Seq<R1s..., R2s...> operator&(Seq<R1s...>, Seq<R2s...>) {
    return {};
}

/// PEG ordered choice `e1 | e2`.
template<RuleType... Rs>
struct Sor {
    DEFINE_PARSE(in, out) {
        if ((Rs::parse(in, out) || ...)) return true;
        return false;
    }
};

template<RuleType R1, RuleType R2>
[[nodiscard]] constexpr Sor<R1, R2> operator|(R1, R2) {
    return {};
}

template<RuleType R1, RuleType... R2s>
[[nodiscard]] constexpr Sor<R1, R2s...> operator|(R1, Sor<R2s...>) {
    return {};
}

template<RuleType... R1s, RuleType R2>
[[nodiscard]] constexpr Sor<R1s..., R2> operator|(Sor<R1s...>, R2) {
    return {};
}

template<RuleType... R1s, RuleType... R2s>
[[nodiscard]] constexpr Sor<R1s..., R2s...> operator|(Sor<R1s...>, Sor<R2s...>) {
    return {};
}

/// Equivalent to `R & *(S & R)`.
template<RuleType R, RuleType S>
[[nodiscard]] constexpr auto list(R r, S s) {
    return r & *(s & r);
}

/// Equivalent to `R & *((P & S & P) & R)`.
template<RuleType R, RuleType S, RuleType P>
[[nodiscard]] constexpr auto list(R r, S s, P p) {
    return r & *((p & s & p) & r);
}

/// Equivalent to `R & S & Rs[0] & S & Rs[1] & ...`.
template<RuleType S, RuleType R, RuleType... Rs>
[[nodiscard]] constexpr auto join(S s, R r, Rs... rs) {
    return r & ((s & rs) & ...);
}

#undef DEFINE_PARSE
#undef MAKE_TOKEN

}  // namespace qcpc
