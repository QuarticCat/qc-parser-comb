# Rule Reference

- [Atomic Rules](#atomic-rules)
- [ASCII Rules](#ascii-rules)
- [Combinators](#combinators)

## Atomic Rules

struct `Boi` / variable `boi`
- Match the beginning of input. Consume nothing.

struct `Eoi` / variable `eoi`
- Match the end of input. Consume nothing.

struct `Bol` / variable `bol`
- Match the beginning of lines. Consume nothing.

struct `Eol` / variable `eol`
- Match the end of lines. Consume "\r\n" or "\n".
  
## ASCII Rules

struct `Str<char...>` / variable `str<char...>`
- Match and consume given string.

## Combinators

struct `At<RuleType>` / operator `&` (unary)
- PEG and-predicate ***&e***.

struct `NotAt<RuleType>` / operator `!`
- PEG not-predicate ***!e***.

struct `Opt<RuleType>` / operator `-` (unary)
- PEG optional ***e?***.

struct `Star<RuleType>` / operator `*` (unary)
- PEG zero-or-more ***e****.

struct `Plus<RuleType>` / operator `+` (unary)
- PEG one-or-more ***e+***.

struct `Silent<RuleType>` / operator `~`
- Match (and consume) silently. The token it returns has no child.

struct `Seq<RuleType...>` / operator `&` (binary)
- PEG sequence ***e1 e2***.

struct `Sor<RuleType...>` / operator `|`
- PEG ordered choice ***e1 | e2***.
