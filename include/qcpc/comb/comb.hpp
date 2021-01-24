#pragma once

#include "parse_ret.hpp"
#include "rule.hpp"

namespace qcpc {

/// A macro helps you define your own rule which has an unique `RuleTag`. All tokens it matches
/// will have this hash tag. You should always define your rules by this macro. Note that this macro
/// can not be used in scopes.
///
/// Usage:
/// ```
/// QCPC_DEFINE_RULE(my_rule) = rule_expression;
/// ```
#define QCPC_DEFINE_RULE(name)                                                   \
    template<::qcpc::RuleType R>                                                 \
    struct GeneratedRule_##name: ::qcpc::RuleBase {                              \
        using Rule = R;                                                          \
                                                                                 \
        /* Can not mark explicit here, it will prevent deduction. */             \
        constexpr GeneratedRule_##name(R) noexcept {}                            \
                                                                                 \
        [[nodiscard]] constexpr size_t tag() const noexcept {                    \
            return _tag;                                                         \
        }                                                                        \
                                                                                 \
        template<bool Silent, ::qcpc::RuleTag = ::qcpc::NO_RULE, typename Input> \
        static ::qcpc::ParseRet parse(Input& in) {                               \
            return R::template parse<Silent, _tag>(in);                          \
        }                                                                        \
                                                                                 \
      private:                                                                   \
        static constexpr size_t _tag = __COUNTER__;                              \
    };                                                                           \
    inline constexpr GeneratedRule_##name name

/// Parse and return token tree.
///
/// Usage:
/// ```
/// auto tokens = parse<decltype(my_rule)>(my_input);
/// ```
template<RuleType T, typename Input>
Token::Ptr parse(Input&& in) {
    ParseRet ret = T::template parse<false>(in);
    return ret.get_ptr();
}

/// A macro simplifies the usage of `parse` function.
///
/// Usage:
/// ```
/// auto tokens = QCPC_PARSE(my_rule, my_input);
/// ```
#define QCPC_PARSE(rule_obj, input) (::qcpc::parse<decltype(rule_obj)>(input))

}  // namespace qcpc
