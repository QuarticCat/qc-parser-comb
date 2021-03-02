# Rule Reference

- [Atomic Rules](#atomic-rules)
  - [Zero-Width Rules](#zero-width-rules)
  - [Basic Rules](#basic-rules)
  - [Predefined Convenient Rules](#predefined-convenient-rules)
- [Combinators](#combinators)

## Atomic Rules

### Zero-Width Rules

struct `Boi` / variable `boi`
- Match the beginning of input. Consume nothing.

struct `Eoi` / variable `eoi`
- Match the end of input. Consume nothing.

struct `Bol` / variable `bol`
- Match the beginning of lines. Consume nothing.

struct `Eol` / variable `eol`
- Match the end of lines. Consume "\r\n" or "\n".

### Basic Rules

struct `One<char>` / variable `one<char>`
- Match and consume a given character.

struct `Str<char...>` / variable `str<char...>`
- Match and consume given string.
- `str<'a', 'b', 'c', 'd'>` means `"abcd"` in PEG.

struct `Range<char...>` / variable `range<char...>`
- Match and consume a character in given ASCII range(s).
- `range<'a', 'z', 'A', 'Z'>` means `[a-zA-Z]` in PEG.

### Predefined Convenient Rules

struct `Alpha` / variable `alpha`
- Match and consume alphabetic letters.
- Equivalent to `[a-zA-Z]+`.

struct `Num` / variable `num`
- Match and consume numbers.
- Equivalent to `[0-9]+`.

struct `AlNum` / variable `alnum`
- Match and consume alphabetic letters and numbers.
- Equivalent to `[a-zA-Z0-9]+`.

struct `Blank` / variable `blank`
- Match and consume blank characters.
- Equivalent to `[ \t\r\n]+`.

## Combinators

struct `At<RuleType>` / operator `&` (unary)
- PEG and-predicate ***&e***.

struct `NotAt<RuleType>` / operator `!`
- PEG not-predicate ***!e***.

struct `Opt<RuleType>` / operator `-` (unary)
- PEG optional ***e?***.

struct `Star<RuleType>` / operator `*` (unary)
- PEG zero-or-more ___e*___.

struct `Plus<RuleType>` / operator `+` (unary)
- PEG one-or-more ***e+***.

struct `Seq<RuleType...>` / operator `&` (binary)
- PEG sequence ***e1 e2***.

struct `Sor<RuleType...>` / operator `|`
- PEG ordered choice ***e1 | e2***.

