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
concept RuleType = std::is_empty_v<T> && requires(MemoryInput& in, Token::Children& out) {
    { T::parse(in, out) } -> std::same_as<bool>;
};
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

/// Packrat parsing
template<RuleType R, InputType Input>
bool mem_parse(Input& in, Token::Children& out, MemMap& mem) noexcept {
    auto key = {in.current(), detail::get_rule_tag<R>()};
    if (mem.contains(key)) {
        in.jump(mem[key]);
        return true;
    } else {
        auto pos = in.pos();
        bool res = R::parse(in, out);
        if (res) mem[key] = pos;
        return res;
    }
}

}  // namespace detail

#define QCPC_DETAIL_DEFINE_MEM_PARSE(rule_name)                                            \
    template<InputType Input>                                                              \
    static bool mem_parse(Input& in, Token::Children& out, detail::MemMap& mem) noexcept { \
        return detail::mem_parse<rule_name>(in, out, mem);                                 \
    }                                                                                      \
    static_assert(true)

#define QCPC_DETAIL_DEFINE_PARSE(in, out) \
    template<InputType Input>             \
    static bool parse(Input& in, Token::Children& out) noexcept

}  // namespace qcpc
