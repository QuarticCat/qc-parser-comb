#pragma once

#include <concepts>

#include "../ctti/ctti.hpp"
#include "../token/token.hpp"

namespace qcpc {

/// Tag class to indicate a class is a rule type. Every rule type should explicitly or implicitly
/// inherit from this class. "Implicitly" means inherit from other legal rule types.
///
/// A rule type should be like:
/// ```
/// struct RuleTemplate: RuleBase {
///     /// Parse and return matched token. If fail, return null pointer.
///     template<TypeHash Rule, typename Input>
///     static Token::Ptr parse(Input& in) {}
/// };
/// ```
struct RuleBase {};

/// Concept to check if a type is a rule type.
template<typename T>
concept RuleType = std::derived_from<T, RuleBase>;
// TODO: check interface

/// This macro helps you define your own rule which has a unique `TypeHash`. All tokens it matches
/// will has this hash tag. You should always define your rules by this macro.
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
        template<TypeHash _, typename Input>                                 \
        static ::qcpc::Token::Ptr parse(Input& in) {                         \
            constexpr auto hash = ::qcpc::type_hash<GeneratedRule_##name>(); \
            return InnerRule::template parse<hash>(in);                      \
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
