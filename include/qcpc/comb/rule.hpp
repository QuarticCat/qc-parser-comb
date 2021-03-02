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

using ParseRet = std::unique_ptr<Token>;

/// Concept to check if a type is a rule type.
// clang-format off
template<class T>
concept RuleType = std::is_empty_v<T> && requires(MemoryInput in) {
    { T::parse(in) } -> std::same_as<ParseRet>;
};
// clang-format on

// This macro is mainly for reducing refactoring workload during early development.
#define DEFINE_PARSE(input_param) \
    template<InputType Input>     \
    static ParseRet parse(Input& input_param) noexcept

// Template function `make_token(Args&&...)` may prevent some type deductions.
#define MAKE_TOKEN(...) ParseRet(new Token(__VA_ARGS__))

// ==================
// || Atomic Rules ||
// ==================

/// Match the beginning of input. Consume nothing.
struct Boi {
    DEFINE_PARSE(in) {
        return in.is_boi() ? MAKE_TOKEN(in.pos()) : nullptr;
    }
};

inline constexpr Boi boi{};

/// Match the end of input. Consume nothing.
struct Eoi {
    DEFINE_PARSE(in) {
        return in.is_eoi() ? MAKE_TOKEN(in.pos()) : nullptr;
    }
};

inline constexpr Eoi eoi{};

/// Match the beginning of lines. Consume nothing.
struct Bol {
    DEFINE_PARSE(in) {
        return in.column() == 0 ? MAKE_TOKEN(in.pos()) : nullptr;
    }
};

inline constexpr Bol bol{};

/// Match the end of lines. Consume "\r\n" or "\n".
struct Eol {
    DEFINE_PARSE(in) {
        auto pos = in.pos();
        if (*in == '\n') {
            ++in;
            goto success;
        }
        if (*in == '\r') {
            if (*++in == '\n') {
                ++in;
                goto success;
            }
            in.jump(pos);
        }
        return nullptr;
    success:
        return MAKE_TOKEN({pos, in.current()});
    }
};

inline constexpr Eol eol{};

/// Match and consume a given character.
template<char C>
struct One {
    DEFINE_PARSE(in) {
        if (*in == C) {
            auto pos = in.pos();
            ++in;
            return MAKE_TOKEN({pos, in.current()});
        } else {
            return nullptr;
        }
    }
};

template<char C>
inline constexpr One<C> one{};

/// Match and consume given string.
template<char C, char... Cs>
struct Str {
    // Waiting for complete support of "Class Types in Non-Type Template Parameters" feature.
    // With this feature, we can pass a string literal as a template parameter.

    DEFINE_PARSE(in) {
        auto pos = in.pos();
        if (in.size() < sizeof...(Cs)) return nullptr;
        if (C == *in && ((Cs == *++in) && ...)) {
            ++in;
            return MAKE_TOKEN({pos, in.current()});
        }
        in.jump(pos);
        return nullptr;
    }
};

template<char C, char... Cs>
inline constexpr Str<C, Cs...> str{};

/// Match and consume a character in given ASCII range(s).
template<char... Cs>
struct Range {
    static_assert(sizeof...(Cs) % 2 == 0, "Param number should be even.");
    // Should we check the validity of ranges?

    DEFINE_PARSE(in) {
        constexpr char cs[] = {Cs...};
        for (size_t i = 0; i < sizeof...(Cs); i += 2) {
            if (cs[i] <= *in && *in <= cs[i + 1]) {
                auto pos = in.pos();
                ++in;
                return MAKE_TOKEN({pos, in.current()});
            }
        }
        return nullptr;
    }
};

template<char... Cs>
inline constexpr Range<Cs...> range{};

namespace detail {

template<InputType Input>
ParseRet light_plus(Input& in, bool (*pred)(char)) {
    if (!pred(*in)) return nullptr;
    auto pos = in.pos();
    do { ++in; } while (pred(*in));
    return MAKE_TOKEN({pos, in.current()});
}

}  // namespace detail

/// Match and consume alphabetic letters. Equivalent to `[a-zA-Z]+`.
struct Alpha {
    DEFINE_PARSE(in) {
        return detail::light_plus(
            in, [](char c) { return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); });
    }
};

inline constexpr Alpha alpha{};

/// Match and consume numbers. Equivalent to `[0-9]+`.
struct Num {
    DEFINE_PARSE(in) {
        return detail::light_plus(in, [](char c) { return '0' <= c && c <= '9'; });
    }
};

inline constexpr Num num{};

/// Match and consume alphabetic letters and numbers. Equivalent to `[a-zA-Z0-9]+`.
struct AlNum {
    DEFINE_PARSE(in) {
        return detail::light_plus(in, [](char c) {
            return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9');
        });
    }
};

inline constexpr AlNum alnum{};

/// Match and consume blank characters. Equivalent to `[ \t\r\n]+`.
struct Blank {
    DEFINE_PARSE(in) {
        return detail::light_plus(
            in, [](char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; });
    }
};

inline constexpr Blank blank{};

// =================
// || Combinators ||
// =================

namespace detail {

struct CombTag {};

template<class T>
concept CombRule = std::derived_from<T, CombTag>;

inline void concat(Token::Children& dst, Token::Children& src) noexcept {
    if (dst.empty()) {
        dst = std::move(src);
    } else {
        dst.reserve(dst.size() + src.size());
        std::move(src.begin(), src.end(), std::back_inserter(dst));
        src.clear();
    }
}

inline ParseRet wrap(ParseRet ret, RuleTag tag = NO_RULE) {
    auto pos = ret->pos();
    Token::Children children{};
    children.push_back(std::move(*ret));
    return MAKE_TOKEN(std::move(children), pos, tag);
}

}  // namespace detail

/// PEG and-predicate `&e`.
template<RuleType R>
struct At: detail::CombTag {
    DEFINE_PARSE(in) {
        auto pos = in.pos();
        auto ret = R::parse(in);
        in.jump(pos);
        return ret ? MAKE_TOKEN(pos) : nullptr;
    }
};

template<RuleType R>
[[nodiscard]] constexpr At<R> operator&(R) {
    return {};
}

/// PEG not-predicate `!e`.
template<RuleType R>
struct NotAt: detail::CombTag {
    DEFINE_PARSE(in) {
        auto pos = in.pos();
        auto ret = R::parse(in);
        in.jump(pos);
        return ret ? nullptr : MAKE_TOKEN(pos);
    }
};

template<RuleType R>
[[nodiscard]] constexpr NotAt<R> operator!(R) {
    return {};
}

/// PEG optional `e?`.
template<RuleType R>
struct Opt: detail::CombTag {
    DEFINE_PARSE(in) {
        auto ret = R::parse(in);
        if (ret) return ret;
        return MAKE_TOKEN(in.pos());
    }
};

template<RuleType R>
[[nodiscard]] constexpr Opt<R> operator-(R) {
    return {};
}

/// PEG zero-or-more `e*`.
template<RuleType R>
struct Star: detail::CombTag {
    DEFINE_PARSE(in) {
        auto pos = in.pos();

        Token::Children children{};
        while (auto ret = R::parse(in)) {
            if constexpr (detail::CombRule<R>) {
                detail::concat(children, ret->children);
            } else {
                children.push_back(std::move(*ret));
            }
        }

        return MAKE_TOKEN(std::move(children), {pos, in.current()});
    }
};

template<RuleType R>
[[nodiscard]] constexpr Star<R> operator*(R) {
    return {};
}

/// PEG one-or-more `e+`.
template<RuleType R>
struct Plus: detail::CombTag {
    DEFINE_PARSE(in) {
        auto pos = in.pos();

        Token::Children children{};
        while (auto ret = R::parse(in)) {
            if constexpr (detail::CombRule<R>) {
                detail::concat(children, ret->children);
            } else {
                children.push_back(std::move(*ret));
            }
        }

        return children.empty() ? nullptr : MAKE_TOKEN(std::move(children), {pos, in.current()});
    }
};

template<RuleType R>
[[nodiscard]] constexpr Plus<R> operator+(R) {
    return {};
}

/// PEG sequence `e1 e2`.
template<RuleType R, RuleType... Rs>
struct Seq: detail::CombTag {
    DEFINE_PARSE(in) {
        auto pos = in.pos();
        auto ret = R::parse(in);
        if (!ret) return nullptr;

        Token::Children children{};
        children.push_back(std::move(*ret));
        if ((_helper<Rs>(in, children) && ...))
            return MAKE_TOKEN(std::move(children), {pos, in.current()});

        in.jump(pos);
        return nullptr;
    }

  private:
    template<RuleType R_, InputType Input>
    static bool _helper(Input& in, Token::Children& children) noexcept {
        auto ret = R_::parse(in);
        if (!ret) return false;
        if constexpr (detail::CombRule<R_>) {
            detail::concat(children, ret->children);
        } else {
            children.push_back(std::move(*ret));
        }
        return true;
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
template<RuleType R, RuleType... Rs>
struct Sor: detail::CombTag {
    DEFINE_PARSE(in) {
        auto ret = _helper<R>(in);
        ret || ((ret = _helper<Rs>(in)) || ...);
        return ret;
    }

  private:
    template<RuleType R_, InputType Input>
    static ParseRet _helper(Input& in) {
        auto ret = R_::parse(in);
        if constexpr (detail::CombRule<R_>) {
            return ret;
        } else {
            return ret ? detail::wrap(std::move(ret)) : nullptr;
        }
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

#undef DEFINE_PARSE
#undef MAKE_TOKEN

}  // namespace qcpc
