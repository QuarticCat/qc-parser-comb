#pragma once

#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <limits>
#include <type_traits>
#include <utility>

#include "../input/input.hpp"
#include "rule_tag.hpp"
#include "token.hpp"

namespace qcpc {

/// Concept to check if a type is a rule type.
// clang-format off
template<class T>
concept RuleType = std::is_empty_v<T> && requires(MemoryInput in) {
    { T::parse(in) } -> std::same_as<Token::Ptr>;
};
// clang-format on

// This macro is mainly for reducing refactoring workload during early development.
#define DEFINE_PARSE(input_param) \
    template<InputType Input>     \
    static Token::Ptr parse(Input& input_param) noexcept

// Template function `make_token(Args&&...)` may prevent some type deductions.
#define MAKE_TOKEN(...) Token::Ptr(new Token(__VA_ARGS__))

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
template<char... Cs>
struct Str {
    // Waiting for complete support of "Class Types in Non-Type Template Parameters" feature.
    // With this feature, we can pass a string literal as a template parameter.

    DEFINE_PARSE(in) {
        auto pos = in.pos();
        if (in.size() < sizeof...(Cs)) return nullptr;
        for (char c: {Cs...}) {
            if (c != *in) {
                in.jump(pos);
                return nullptr;
            }
            ++in;
        }
        return MAKE_TOKEN({pos, in.current()});
    }
};

template<char... Cs>
inline constexpr Str<Cs...> str{};

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

// /// Match and consume alphabetic letters. Equivalent to `[a-zA-Z]+`.
// struct Alpha {
//     // TODO
// };
//
// inline constexpr Alpha alpha{};
//
// /// Match and consume numbers. Equivalent to `[0-9]+`.
// struct Num {
//     // TODO
// };
//
// inline constexpr Num num{};
//
// /// Match and consume alphabetic letters and numbers. Equivalent to `[a-zA-Z0-9]+`.
// struct AlNum {
//     // TODO
// };
//
// inline constexpr AlNum alnum{};

/// PEG and-predicate `&e`.
template<RuleType R>
struct At {
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
struct NotAt {
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
struct Opt {
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
struct Star {
    DEFINE_PARSE(in) {
        auto pos = in.pos();
        auto head = R::parse(in);
        if (!head) return MAKE_TOKEN(pos);
        Token* tail = head.get();
        while (true) {
            auto ret = R::parse(in);
            if (!ret) break;
            tail->link(std::move(ret));
            tail = tail->next();
        }
        return MAKE_TOKEN(std::move(head), {pos, in.current()});
    }
};

template<RuleType R>
[[nodiscard]] constexpr Star<R> operator*(R) {
    return {};
}

/// PEG one-or-more `e+`.
template<RuleType R>
struct Plus {
    DEFINE_PARSE(in) {
        auto pos = in.pos();
        auto head = R::parse(in);
        if (!head) return nullptr;
        Token* tail = head.get();
        while (true) {
            auto ret = R::parse(in);
            if (!ret) break;
            tail->link(std::move(ret));
            tail = tail->next();
        }
        return MAKE_TOKEN(std::move(head), {pos, in.current()});
    }
};

template<RuleType R>
[[nodiscard]] constexpr Plus<R> operator+(R) {
    return {};
}

/// PEG sequence `e1 e2`.
template<RuleType R, RuleType... Rs>
struct Seq {
    DEFINE_PARSE(in) {
        auto pos = in.pos();
        auto head = R::parse(in);
        if (!head) return nullptr;
        Token* tail = head.get();
        // Type hint is necessary for GCC.
        using ParseList = std::initializer_list<Token::Ptr (*)(Input&)>;
        for (auto f: ParseList{Rs::template parse<Input>...}) {
            auto ret = f(in);
            if (!ret) {
                in.jump(pos);
                return nullptr;
            }
            tail->link(std::move(ret));
            tail = tail->next();
        }
        return MAKE_TOKEN(std::move(head), {pos, in.current()});
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
    DEFINE_PARSE(in) {
        // Type hint is necessary for GCC.
        using ParseList = std::initializer_list<Token::Ptr (*)(Input&)>;
        for (auto f: ParseList{Rs::template parse<Input>...}) {
            auto ret = f(in);
            if (ret) return ret;
        }
        return nullptr;
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
