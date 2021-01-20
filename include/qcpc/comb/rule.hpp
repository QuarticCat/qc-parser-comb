#pragma once

#include <concepts>

#include "../ctti/ctti.hpp"
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

/// This macro helps you define your own rule which has an unique `TypeHash`. All tokens it matches
/// will have this hash tag. You should always define your rules by this macro.
///
/// Usage:
/// ```
/// DEFINE_RULE(my_rule) = rule_expression;
/// // `my_rule` then can be used in parsing and in other rule expressions.
/// ```
#define DEFINE_RULE(name)                                                    \
    template<::qcpc::RuleType InnerRule>                                     \
    struct GeneratedRule_##name: ::qcpc::RuleBase {                          \
        constexpr GeneratedRule_##name(InnerRule) noexcept {}                \
                                                                             \
        template<TypeHash /* Rule */, bool Silent, typename Input>           \
        static ::qcpc::ParseRet parse(Input& in) {                           \
            constexpr auto hash = ::qcpc::type_hash<GeneratedRule_##name>(); \
            return InnerRule::template parse<hash, Silent>(in);              \
        }                                                                    \
    };                                                                       \
    inline constexpr GeneratedRule_##name name

// ############
// Atomic Rules
// ############

// ###########
// Combinators
// ###########

}  // namespace qcpc
