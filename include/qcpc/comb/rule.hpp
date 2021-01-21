#pragma once

#include <concepts>
#include <cstddef>

#include "../ctti/ctti.hpp"
#include "../input/input.hpp"
#include "../token/token.hpp"
#include "parse_ret.hpp"

namespace qcpc {

/// Tag class to indicate a class is a rule type. Every rule type should explicitly or implicitly
/// inherit from this class. "Implicitly" means inherit from other legal rule types.
///
/// A rule type should be like:
/// ```
/// struct RuleTemplate: RuleBase {
///     /// Parse and return matched token (or just bool). To see what should be returned, please
///     /// refer to the doc of `ParseRet`. This function is also responsible to recover the input
///     /// if parse fail.
///     template<TypeHash Rule, bool Silent, typename Input>
///     static ParseRet parse(Input& in) {}
/// };
/// ```
struct RuleBase {};

/// Concept to check if a type is a rule type.
template<typename T>
concept RuleType = std::derived_from<T, RuleBase>;
// TODO: check interface

namespace detail {

constexpr ParseRet match_failed(bool silent) {
    return silent ? ParseRet(false) : ParseRet(nullptr);
}

}  // namespace detail

/// Match begin of file. Consume nothing.
struct Bof: RuleBase {
    template<TypeHash Rule, bool Silent, typename Input>
    static ParseRet parse(Input& in) {
        if constexpr (Silent) {
            return ParseRet(in.is_bof());
        } else {
            return in.is_bof() ? ParseRet(new Token(in.pos(), Rule)) : ParseRet(nullptr);
        }
    }
};

inline constexpr Bof bof{};

/// Match end of file. Consume nothing.
struct Eof: RuleBase {
    template<TypeHash Rule, bool Silent, typename Input>
    static ParseRet parse(Input& in) {
        if constexpr (Silent) {
            return ParseRet(in.is_eof());
        } else {
            return in.is_eof() ? ParseRet(new Token(in.pos(), Rule)) : ParseRet(nullptr);
        }
    }
};

inline constexpr Eof eof{};

/// Match and consume given string.
template<char... Cs>
struct Str: RuleBase {
    template<TypeHash Rule, bool Silent, typename Input>
    static ParseRet parse(Input& in) {
        InputPos init_pos = in.pos();

        if (in.size() >= sizeof...(Cs)) {
            for (char c: {Cs...}) {
                if (c != *in) return detail::match_failed(Silent);
                ++in;
            }
        } else {
            return detail::match_failed(Silent);
        }

        if constexpr (Silent) {
            return ParseRet(true);
        } else {
            TokenPos last_pos(init_pos, in.current());
            return ParseRet(new Token(last_pos, Rule));
        }
    }
};

template<char... Cs>
inline constexpr Str<Cs...> str{};

/// Match given rule but consume nothing.
template<RuleType R>
struct At: RuleBase {
    template<TypeHash Rule, bool Silent, typename Input>
    static ParseRet parse(Input& in) {
        InputPos pos = in.pos();
        ParseRet ret = R::template parse<0, Silent>(in);
        in.jump(pos);
        if constexpr (Silent) {
            return ParseRet(ret.success());
        } else {
            return ret.success() ? ParseRet(new Token(pos, Rule)) : ParseRet(nullptr);
        }
    }
};

template<RuleType R>
constexpr At<R> operator+(R) {
    return At<R>{};
}

/// Not match given rule and consume nothing.
template<RuleType R>
struct NotAt: RuleBase {
    template<TypeHash Rule, bool Silent, typename Input>
    static ParseRet parse(Input& in) {
        InputPos pos = in.pos();
        ParseRet ret = R::template parse<0, Silent>(in);
        in.jump(pos);
        if constexpr (Silent) {
            return ParseRet(!ret.success());
        } else {
            return ret.success() ? ParseRet(nullptr) : ParseRet(new Token(pos, Rule));
        }
    }
};

template<RuleType R>
constexpr NotAt<R> operator-(R) {
    return NotAt<R>{};
}

}  // namespace qcpc
