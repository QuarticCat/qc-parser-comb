#pragma once

#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <limits>
#include <type_traits>
#include <utility>

#include "../input/input.hpp"
#include "../token/token.hpp"
#include "rule_tag.hpp"
#include "parse_ret.hpp"

namespace qcpc {

/// Concept to check if a type is a rule type.
///
/// A rule type should be empty and has a parse function, like:
/// ```
/// struct RuleTemplate {
///     /// Parse and return matched token (or just bool). To see what should be returned, please
///     /// refer to the doc of `ParseRet`. This function is also responsible to recover the input
///     /// if parse fail.
///     template<bool Silent, RuleTag Tag = NO_RULE, typename Input>
///     static ParseRet parse(Input& in) {}
/// };
/// ```
// clang-format off
template<typename T>
concept RuleType = std::is_empty_v<T> && requires(T) {
    { T::template parse<false, NO_RULE, MemoryInput>(std::declval<MemoryInput&>()) }
        -> std::same_as<ParseRet>;
};
// clang-format on

/// This macro is mainly for reducing refactoring workload during early development. So it is under
/// `QCPC_DETAIL` and library users could not use it.
#define QCPC_DETAIL_DEFINE_PARSE(input_param)                    \
    template<bool Silent, RuleTag Tag = NO_RULE, typename Input> \
    static ParseRet parse(Input& input_param)

namespace detail {

constexpr ParseRet match_failed(bool silent) {
    return silent ? ParseRet(false) : ParseRet(nullptr);
}

}  // namespace detail

/// Match the beginning of input. Consume nothing.
struct Boi {
    QCPC_DETAIL_DEFINE_PARSE(in) {
        if constexpr (Silent) {
            return ParseRet(in.is_bof());
        } else {
            return in.is_bof() ? ParseRet(new Token(in.pos(), Tag)) : ParseRet(nullptr);
        }
    }
};

inline constexpr Boi boi{};

/// Match the end of input. Consume nothing.
struct Eoi {
    QCPC_DETAIL_DEFINE_PARSE(in) {
        if constexpr (Silent) {
            return ParseRet(in.is_eof());
        } else {
            return in.is_eof() ? ParseRet(new Token(in.pos(), Tag)) : ParseRet(nullptr);
        }
    }
};

inline constexpr Eoi eoi{};

/// Match the beginning of lines. Consume nothing.
struct Bol {
    QCPC_DETAIL_DEFINE_PARSE(in) {
        if constexpr (Silent) {
            return ParseRet(in.column() == 0);
        } else {
            return in.column() == 0 ? ParseRet(new Token(in.pos(), Tag)) : ParseRet(nullptr);
        }
    }
};

inline constexpr Bol bol{};

/// Match the end of lines. Consume "\r\n" or "\n".
struct Eol {
    QCPC_DETAIL_DEFINE_PARSE(in) {
        InputPos pos = in.pos();
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
        return detail::match_failed(Silent);
    success:
        if constexpr (Silent) {
            return ParseRet(true);
        } else {
            return ParseRet(new Token({pos, in.current()}, Tag));
        }
    }
};

inline constexpr Eol eol{};

/// Match and consume given string.
template<char... Cs>
struct Str {
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
            return ParseRet(new Token({pos, in.current()}, Tag));
        }
    }
};

template<char... Cs>
inline constexpr Str<Cs...> str{};

/// PEG and-predicate `&e`.
template<RuleType R>
struct At {
    QCPC_DETAIL_DEFINE_PARSE(in) {
        InputPos pos = in.pos();
        ParseRet ret = R::template parse<Silent>(in);
        in.jump(pos);
        if constexpr (Silent) {
            return ParseRet(ret.success());
        } else {
            return ret.success() ? ParseRet(new Token({pos}, Tag)) : ParseRet(nullptr);
        }
    }
};

template<RuleType R>
[[nodiscard]] constexpr At<R> operator&(R) {
    return {};
}

/// PEG not-predicate `!e`.
template<RuleType R>
struct NotAt {
    QCPC_DETAIL_DEFINE_PARSE(in) {
        InputPos pos = in.pos();
        ParseRet ret = R::template parse<Silent>(in);
        in.jump(pos);
        if constexpr (Silent) {
            return ParseRet(!ret.success());
        } else {
            return ret.success() ? ParseRet(nullptr) : ParseRet(new Token({pos}, Tag));
        }
    }
};

template<RuleType R>
[[nodiscard]] constexpr NotAt<R> operator!(R) {
    return {};
}

/// PEG optional `e?`.
template<RuleType R>
struct Opt {
    QCPC_DETAIL_DEFINE_PARSE(in) {
        if constexpr (Silent) {
            R::template parse<Silent>(in);
            return ParseRet(true);
        } else {
            ParseRet ret = R::template parse<Silent>(in);
            if (ret.success()) return ret;
            return ParseRet(new Token({in.pos()}, Tag));
        }
    }
};

template<RuleType R>
[[nodiscard]] constexpr Opt<R> operator-(R) {
    return {};
}

/// PEG zero-or-more `e*`.
template<RuleType R>
struct Star {
    QCPC_DETAIL_DEFINE_PARSE(in) {
        if constexpr (Silent) {
            while (R::template parse<Silent>(in).get_result()) {}
            return ParseRet(true);
        } else {
            InputPos pos = in.pos();
            Token::Ptr head = R::template parse<Silent>(in).get_ptr();
            if (!head) return ParseRet(new Token({pos}, Tag));  // no need to jump
            Token* tail = head.get();  // A token holds the ownership, so it is a raw ptr.
            while (true) {
                Token::Ptr ret = R::template parse<Silent>(in).get_ptr();
                if (!ret) break;
                tail->link(std::move(ret));
                tail = tail->next();
            }
            return ParseRet(new Token(std::move(head), {pos, in.current()}, Tag));
        }
    }
};

template<RuleType R>
[[nodiscard]] constexpr Star<R> operator*(R) {
    return {};
}

/// PEG one-or-more `e+`.
template<RuleType R>
struct Plus {
    QCPC_DETAIL_DEFINE_PARSE(in) {
        if constexpr (Silent) {
            ParseRet first = R::template parse<Silent>(in);
            if (!first.get_result()) return first;
            while (R::template parse<Silent>(in).get_result()) {}
            return ParseRet(true);
        } else {
            InputPos pos = in.pos();
            Token::Ptr head = R::template parse<Silent>(in).get_ptr();
            if (!head) return ParseRet(nullptr);
            Token* tail = head.get();
            while (true) {
                Token::Ptr ret = R::template parse<Silent>(in).get_ptr();
                if (!ret) break;
                tail->link(std::move(ret));
                tail = tail->next();
            }
            return ParseRet(new Token(std::move(head), {pos, in.current()}, Tag));
        }
    }
};

template<RuleType R>
[[nodiscard]] constexpr Plus<R> operator+(R) {
    return {};
}

/// Match (and consume) silently. The token it returns has no child.
template<RuleType R>
struct Silent {
    QCPC_DETAIL_DEFINE_PARSE(in) {
        if constexpr (Silent) {
            return R::template parse<true>(in);
        } else {
            InputPos pos = in.pos();
            ParseRet ret = R::template parse<true>(in);
            return ret.success() ? ParseRet(new Token({pos, in.current()}, Tag))
                                 : ParseRet(nullptr);
        }
    }
};

template<RuleType R>
[[nodiscard]] constexpr Silent<R> operator~(R) {
    return {};
}

/// PEG sequence `e1 e2`.
template<RuleType R, RuleType... Rs>
struct Seq {
    QCPC_DETAIL_DEFINE_PARSE(in) {
        InputPos pos = in.pos();
        if constexpr (Silent) {
            bool result = (R::template parse<Silent>(in).get_result() && ... &&
                           Rs::template parse<Silent>(in).get_result());
            if (!result) in.jump(pos);
            return ParseRet(result);
        } else {
            Token::Ptr head = R::template parse<Silent>(in).get_ptr();
            if (!head) return ParseRet(nullptr);
            Token* tail = head.get();
            // Type hint is necessary for GCC.
            using ParseList = std::initializer_list<ParseRet (*)(Input&)>;
            for (auto f: ParseList{Rs::template parse<Silent, NO_RULE, Input>...}) {
                Token::Ptr ret = f(in).get_ptr();
                if (!ret) {
                    in.jump(pos);
                    return ParseRet(nullptr);
                }
                tail->link(std::move(ret));
                tail = tail->next();
            }
            return ParseRet(new Token(std::move(head), {pos, in.current()}, Tag));
        }
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
    QCPC_DETAIL_DEFINE_PARSE(in) {
        if constexpr (Silent) {
            return ParseRet((... || Rs::template parse<Silent>(in).get_result()));
        } else {
            // Type hint is necessary for GCC.
            using ParseList = std::initializer_list<ParseRet (*)(Input&)>;
            for (auto f: ParseList{Rs::template parse<Silent, Tag, Input>...}) {
                ParseRet ret = f(in);
                if (ret.success()) return ret;
            }
            return ParseRet(nullptr);
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

#undef QCPC_DETAIL_DEFINE_PARSE

}  // namespace qcpc
