#pragma once

#include <concepts>
#include <cstddef>
#include <limits>

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
///     template<bool Silent, RuleTag Tag = NO_RULE, typename Input>
///     static ParseRet parse(Input& in) {}
/// };
/// ```
struct RuleBase {};

/// This macro is mainly for reducing refactoring workload during early development. So it is under
/// `QCPC_DETAIL` and library users should not use it.
#define QCPC_DETAIL_DEFINE_PARSE(input_param)                    \
    template<bool Silent, RuleTag Tag = NO_RULE, typename Input> \
    static ParseRet parse(Input& input_param)
// TODO: Should tparam `Silent` have a default value?

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
    QCPC_DETAIL_DEFINE_PARSE(in) {
        if constexpr (Silent) {
            return ParseRet(in.is_bof());
        } else {
            return in.is_bof() ? ParseRet(new Token(in.pos(), Tag)) : ParseRet(nullptr);
        }
    }
};

inline constexpr Bof bof{};

/// Match end of file. Consume nothing.
struct Eof: RuleBase {
    QCPC_DETAIL_DEFINE_PARSE(in) {
        if constexpr (Silent) {
            return ParseRet(in.is_eof());
        } else {
            return in.is_eof() ? ParseRet(new Token(in.pos(), Tag)) : ParseRet(nullptr);
        }
    }
};

inline constexpr Eof eof{};

/// Match and consume given string.
template<char... Cs>
struct Str: RuleBase {
    // Waiting for complete support of "Class Types in Non-Type Template Parameters" feature.
    // With this feature, we can pass a string literal as a template parameter.

    QCPC_DETAIL_DEFINE_PARSE(in) {
        InputPos pos = in.pos();

        if (in.size() >= sizeof...(Cs)) {
            for (char c: {Cs...}) {
                if (c != *in) {
                    in.jump(pos);
                    return detail::match_failed(Silent);
                }
                ++in;
            }
        } else {
            return detail::match_failed(Silent);
        }

        if constexpr (Silent) {
            return ParseRet(true);
        } else {
            return ParseRet(new Token(pos, in.current(), Tag));
        }
    }
};

template<char... Cs>
inline constexpr Str<Cs...> str{};

/// Match given rule but consume nothing.
template<RuleType R>
struct At: RuleBase {
    QCPC_DETAIL_DEFINE_PARSE(in) {
        InputPos pos = in.pos();
        ParseRet ret = R::template parse<Silent>(in);
        in.jump(pos);
        if constexpr (Silent) {
            return ParseRet(ret.success());
        } else {
            return ret.success() ? ParseRet(new Token(pos, Tag)) : ParseRet(nullptr);
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
    QCPC_DETAIL_DEFINE_PARSE(in) {
        InputPos pos = in.pos();
        ParseRet ret = R::template parse<Silent>(in);
        in.jump(pos);
        if constexpr (Silent) {
            return ParseRet(!ret.success());
        } else {
            return ret.success() ? ParseRet(nullptr) : ParseRet(new Token(pos, Tag));
        }
    }
};

template<RuleType R>
constexpr NotAt<R> operator-(R) {
    return NotAt<R>{};
}

/// Match (and consume) silently. The token it returns has no child.
template<RuleType R>
struct Silent: RuleBase {
    QCPC_DETAIL_DEFINE_PARSE(in) {
        if constexpr (Silent) {
            return R::template parse<Silent>(in);
        } else {
            InputPos pos = in.pos();
            ParseRet ret = R::template parse<Silent>(in);
            return ret.success() ? ParseRet(new Token(pos, in.current(), Tag)) : ParseRet(nullptr);
        }
    }
};

template<RuleType R>
constexpr Silent<R> operator~(R) {
    return Silent<R>{};
}

}  // namespace qcpc
