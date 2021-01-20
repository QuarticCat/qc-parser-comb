#pragma once

#include <concepts>
#include <cstddef>

#include "../ctti/ctti.hpp"
#include "../input/input.hpp"
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

// // An unavailable `Str` implementation due to some limitations of C++.
// // I leave it here in memory of my lost youth.
// template<const char* Pattern>
// struct Str: RuleBase {
//     template<TypeHash Rule, bool Silent, typename Input>
//     static ParseRet parse(Input& in) {
//         InputPos init_pos = in.pos();
//
//         for (const char* i = Pattern; *i != '\0'; ++i) {
//             if (*i == *++in) continue;
//             in.jump(init_pos);
//             if constexpr (Silent)
//                 return ParseRet(false);
//             else
//                 return ParseRet(nullptr);
//         }
//
//         if constexpr (Silent) {
//             return ParseRet(true);
//         } else {
//             TokenPos last_pos{init_pos.current, in.current(), init_pos.line, init_pos.column};
//             return ParseRet(new Token(last_pos, Rule));
//         }
//     }
//
//     // private:
//     //   static constexpr size_t _strlen(const char* cstr) {
//     //       size_t ret = 0;
//     //       while (cstr[ret] != '\0') ++ret;
//     //       return ret;
//     //   }
//     //
//     //   static constexpr size_t _len = _strlen(Pattern);
// };

}  // namespace qcpc
