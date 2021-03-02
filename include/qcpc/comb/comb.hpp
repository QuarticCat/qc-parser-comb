#pragma once

#include <concepts>
#include <memory>
#include <optional>
#include <type_traits>
#include <vector>

#include "rule.hpp"
#include "rule_tag.hpp"
#include "token.hpp"

namespace qcpc {

namespace detail {

struct GeneratedTag {};

template<class T>
concept GeneratedRule = std::derived_from<T, GeneratedTag>;

template<class>
inline constexpr int rule_set = 0;

}  // namespace detail

#define QCPC_DETAIL_MANGLE(name) QCPC_GeneratedRule_##name

#define QCPC_DETAIL_DECL(name, silent)                                                  \
    struct QCPC_DETAIL_MANGLE(name): ::qcpc::detail::GeneratedTag {                     \
        /* Using `__COUNTER__` here may violate ODR. */                                 \
        static constexpr ::qcpc::RuleTag tag =                                          \
            ::qcpc::detail::get_tag<const QCPC_DETAIL_MANGLE(name)>();                  \
        static constexpr bool is_silent = silent;                                       \
                                                                                        \
        /* The use of the inline variable here is IFNDR. */                             \
        template<::qcpc::InputType Input, class Lazy = const QCPC_DETAIL_MANGLE(name)>  \
        friend bool parse_detail(Input& in,                                             \
                                 ::qcpc::Token::Children& out,                          \
                                 QCPC_DETAIL_MANGLE(name)) {                            \
            const auto rule = ::qcpc::detail::rule_set<Lazy>;                           \
            if constexpr (is_silent) {                                                  \
                return rule.parse(in, out);                                             \
            } else {                                                                    \
                Token::Children children{};                                             \
                auto pos = in.pos();                                                    \
                if (rule.parse(in, children)) {                                         \
                    out.emplace_back(                                                   \
                        std::move(children), ::qcpc::TokenPos(pos, in.current()), tag); \
                    return true;                                                        \
                } else {                                                                \
                    return false;                                                       \
                }                                                                       \
            }                                                                           \
        }                                                                               \
                                                                                        \
        template<::qcpc::InputType Input>                                               \
        static bool parse(Input& in, ::qcpc::Token::Children& out) noexcept {           \
            return parse_detail(in, out, QCPC_DETAIL_MANGLE(name){});                   \
        }                                                                               \
    };                                                                                  \
                                                                                        \
    inline constexpr QCPC_DETAIL_MANGLE(name) name {}

/// Declare a regular rule.
#define QCPC_DECL(name) QCPC_DETAIL_DECL(name, false)

/// Declare a silent rule.
#define QCPC_DECL_(name) QCPC_DETAIL_DECL(name, true)

/// Define a rule. The name must be declared by `QCPC_DECL` before.
#define QCPC_DEF(name) \
    template<>         \
    inline constexpr auto ::qcpc::detail::rule_set<const QCPC_DETAIL_MANGLE(name)>

/// A convenient macro that combines `QCPC_DECL` and `QCPC_DEF`.
#define QCPC_DECL_DEF(name) \
    QCPC_DECL(name);        \
    QCPC_DEF(name)

/// A convenient macro that combines `QCPC_DECL_` and `QCPC_DEF`.
#define QCPC_DECL_DEF_(name) \
    QCPC_DECL_(name);        \
    QCPC_DEF(name)

namespace detail {

template<class T>
concept UserInput = InputType<std::remove_reference_t<T>>;

}  // namespace detail

// clang-format off
template<detail::GeneratedRule Rule, detail::UserInput Input>
std::optional<Token> parse(Rule, Input&& in) requires (!Rule::is_silent) {
    Token::Children children{};
    if (Rule::parse(in, children))
        return std::move(children[0]);
    else
        return std::nullopt;
}
// clang-format on

}  // namespace qcpc
