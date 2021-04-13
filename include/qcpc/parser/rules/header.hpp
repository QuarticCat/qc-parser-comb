#pragma once

#include <concepts>
#include <functional>
#include <tuple>
#include <type_traits>
#include <unordered_map>

#include "../../input/input.hpp"
#include "../rule_tag.hpp"
#include "../token.hpp"

namespace qcpc {

/// Concept to check if a type is a rule type.
template<class T>
concept RuleType = std::is_empty_v<T>;
static_assert(true);  // fuck clang-format

namespace detail {

using MemKey = std::tuple<const char*, RuleTag>;

struct MemKeyHash {
    size_t operator()(MemKey key) const noexcept {
        auto [pos, rule] = key;
        auto h1 = std::hash<decltype(pos)>{}(pos);
        auto h2 = std::hash<decltype(rule)>{}(rule);
        return h1 ^ (h2 << 1);
    }
};

using MemMap = std::unordered_map<MemKey, InputPos, MemKeyHash>;

}  // namespace detail

#define QCPC_DETAIL_DEFINE_PARSE(rule_name)                                            \
    /* Packrat parsing */                                                              \
    template<InputType Input>                                                          \
    static bool parse(Input& in, Token::Children& out, detail::MemMap& mem) noexcept { \
        detail::MemKey key{in.current(), detail::rule_tag<rule_name>()};             \
        if (mem.contains(key)) {                                                       \
            in.jump(mem[key]);                                                         \
            return true;                                                               \
        } else {                                                                       \
            auto pos = in.pos();                                                       \
            bool res = parse_detail(in, out, mem);                                     \
            if (res) mem[key] = pos;                                                   \
            return res;                                                                \
        }                                                                              \
    }                                                                                  \
                                                                                       \
    template<InputType Input>                                                          \
    static bool parse_detail(Input& in,                                                \
                             [[maybe_unused]] Token::Children& out,                    \
                             [[maybe_unused]] detail::MemMap& mem) noexcept

// TODO: do we need `State&&` here?

}  // namespace qcpc
