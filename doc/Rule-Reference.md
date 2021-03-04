# Rule Reference

- [Zero-Width Rules](#zero-width-rules)
- [ASCII Rules](#ascii-rules)
- [Combinators](#combinators)
- [Convenient Functions](#convenient-functions)

## Zero-Width Rules

`boi`
- Match the beginning of input. Consume nothing.

`eoi`
- Match the end of input. Consume nothing.

`bol`
- Match the beginning of lines. Consume nothing.

`eol`
- Match the end of lines. Consume "\r\n" or "\n".

## ASCII Rules

`one<char...>`
- Match and consume any given character once.
- `one<'a', 'b', 'c'>` means `[abc]` in PEG.

`str<char...>` / `QCPC_STR(str)`
- Match and consume given string.
- `str<'a', 'b', 'c', 'd'>` means `"abcd"` in PEG.
- `QCPC_STR("abcd")` means `"abcd"` in PEG.

struct `Range<char...>` / variable `range<char...>`
- Match and consume a character in given ASCII range(s).
`[a-zA-Z]` in PEG.

## Combinators

`operator&` (unary)
- PEG and-predicate ***&e***.

`operator!`
- PEG not-predicate ***!e***.

`operator-` (unary)
- PEG optional ***e?***.

`operator*` (unary)
- PEG zero-or-more ___e*___.

`operator+` (unary)
- PEG one-or-more ***e+***.

`operator&` (binary)
- PEG sequence ***e1 e2***.

`operator|`
- PEG ordered choice ***e1 | e2***.

## Convenient Functions

`list(R, S)`
- Equivalent to `R & *(S & R)`.

`list(R, S, P)`
- Equivalent to `R & *((P & S & P) & R)`.

`join(S, R, Rs...)`
- Equivalent to `R & S & Rs[0] & S & Rs[1] & ...`.
