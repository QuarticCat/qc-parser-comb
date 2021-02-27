#pragma once

#include <type_traits>

#include "rule.hpp"
#include "rule_tag.hpp"
#include "token.hpp"

namespace qcpc {

namespace detail {

template<class>
inline constexpr int rule_set = 0;

}  // namespace detail

#define QCPC_DETAIL_MANGL(name) QCPC_GeneratedRule_##name

#define QCPC_DECL(name)                                                               \
    struct QCPC_DETAIL_MANGL(name) {                                                  \
        /* Using `__COUNTER__` here may violate ODR. */                               \
        static constexpr ::qcpc::RuleTag tag =                                        \
            ::qcpc::detail::get_tag<const QCPC_DETAIL_MANGL(name)>();                 \
                                                                                      \
        /* The use of the inline variable here is IFNDR. */                           \
        template<::qcpc::InputType Input, class Lazy = const QCPC_DETAIL_MANGL(name)> \
        friend ::qcpc::Token::Ptr parse_detail(Input& in, QCPC_DETAIL_MANGL(name)) {  \
            return ::qcpc::detail::rule_set<Lazy>.parse(in);                          \
        }                                                                             \
                                                                                      \
        template<::qcpc::InputType Input>                                             \
        static ::qcpc::Token::Ptr parse(Input& in) noexcept {                         \
            auto ptr = parse_detail(in, QCPC_DETAIL_MANGL(name){});                   \
            if (ptr) ::qcpc::detail::set_tag(*ptr, tag);                              \
            return ptr;                                                               \
        }                                                                             \
    };                                                                                \
                                                                                      \
    inline constexpr QCPC_DETAIL_MANGL(name) name {}

/// Define rule. The name must be declared by `QCPC_DECL` before.
#define QCPC_DEF(name) \
    template<>         \
    inline constexpr auto ::qcpc::detail::rule_set<const QCPC_DETAIL_MANGL(name)>

/// A convenient macro that combines `QCPC_DECL` and `QCPC_DEF`.
#define QCPC_DECL_DEF(name) \
    QCPC_DECL(name);        \
    QCPC_DEF(name)

template<RuleType Rule, class Input>
Token::Ptr parse(Rule, Input&& in) requires InputType<std::remove_reference_t<Input>> {
    return Rule::parse(in);
}

}  // namespace qcpc
