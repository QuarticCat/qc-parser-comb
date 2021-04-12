#pragma once

#include <concepts>
#include <type_traits>

#include "../../input/input.hpp"
#include "../rule_tag.hpp"
#include "../token.hpp"

namespace qcpc {

/// Concept to check if a type is a rule type.
// clang-format off
template<class T>
concept RuleType = std::is_empty_v<T> && requires(MemoryInput& in, Token::Children& out) {
    { T::parse(in, out) } -> std::same_as<bool>;
};
// clang-format on

#define QCPC_DETAIL_DEFINE_PARSE(in, out) \
    template<InputType Input> \
    static bool parse(Input& in, Token::Children& out) noexcept

}  // namespace qcpc
