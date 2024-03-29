#pragma once

#include <concepts>
#include <memory>
#include <optional>
#include <type_traits>
#include <vector>

#include "rule_tag.hpp"
#include "rules/rules.hpp"
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

#define QCPC_DETAIL_DECL(name, silent)                                                           \
    struct QCPC_DETAIL_MANGLE(name): ::qcpc::detail::GeneratedTag {                              \
        using Self = QCPC_DETAIL_MANGLE(name);                                                   \
                                                                                                 \
        constexpr static bool is_silent = silent;                                                \
        constexpr static auto tag = silent ? ::qcpc::NO_RULE : ::qcpc::detail::rule_tag<Self>(); \
                                                                                                 \
        /* The use of the inline variable here is IFNDR. */                                      \
        template<::qcpc::InputType Input, class Lazy = Self>                                     \
        friend bool parse_detail(Input& in,                                                      \
                                 ::qcpc::Token::Children& out,                                   \
                                 ::qcpc::detail::MemMap mem,                                     \
                                 Self) {                                                         \
            const auto rule = ::qcpc::detail::rule_set<Lazy>;                                    \
            if constexpr (is_silent) {                                                           \
                return rule.parse(in, out, mem);                                                 \
            } else {                                                                             \
                ::qcpc::Token::Children children{};                                              \
                auto pos = in.pos();                                                             \
                bool res = rule.parse(in, children, mem);                                        \
                if (res) out.push_back({std::move(children), {pos, in.current()}, tag});         \
                return res;                                                                      \
            }                                                                                    \
        }                                                                                        \
                                                                                                 \
        template<::qcpc::InputType Input>                                                        \
        static bool parse(Input& in,                                                             \
                          ::qcpc::Token::Children& out,                                          \
                          ::qcpc::detail::MemMap mem) noexcept {                                 \
            return parse_detail(in, out, mem, Self{});                                           \
        }                                                                                        \
    };                                                                                           \
                                                                                                 \
    inline constexpr QCPC_DETAIL_MANGLE(name) name {}

/// Declare a regular rule.
#define QCPC_DECL(name) QCPC_DETAIL_DECL(name, false)

/// Declare a silent rule.
#define QCPC_DECL_(name) QCPC_DETAIL_DECL(name, true)

/// Define a rule. The name must be declared by `QCPC_DECL` before.
#define QCPC_DEF(name) \
    template<>         \
    inline constexpr auto ::qcpc::detail::rule_set<QCPC_DETAIL_MANGLE(name)>

/// A convenient macro that combines `QCPC_DECL` and `QCPC_DEF`.
#define QCPC_DECL_DEF(name) \
    QCPC_DECL(name);        \
    QCPC_DEF(name)

/// A convenient macro that combines `QCPC_DECL_` and `QCPC_DEF`.
#define QCPC_DECL_DEF_(name) \
    QCPC_DECL_(name);        \
    QCPC_DEF(name)

/// The sole parsing entry for users. It does not allow `Input&&` because the returned `Token`s
/// holds views into the input object, so it must outlive the parse function.
template<detail::GeneratedRule Rule, InputType Input>
std::optional<Token> parse(Rule, Input& in) requires(!Rule::is_silent) {
    Token::Children children{};
    detail::MemMap mem{};
    if (Rule::parse(in, children, mem))
        return std::move(children[0]);
    else
        return std::nullopt;
}

}  // namespace qcpc
