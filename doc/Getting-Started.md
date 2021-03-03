# Getting Started

- [Introduction](#introduction)
- [Defining Rules](#defining-rules)
- [Parsing](#defining-rules)
- [Processing](#processing)

## Introduction

As the name expresses, this library is a parser combinator library.

Parser combinators correspond to
[Parsing Expression Grammars](https://en.wikipedia.org/wiki/Parsing_expression_grammar).
This tutorial assumes you already know what PEG is.

The complete code of this tutorial is in [/examples/calculator](/examples/calculator.cpp).

## Defining Rules

First you need to define your grammar. This library creates a embedded DSL
to help you doing this. A grammar rule definition is comprised of two parts:

- Declaration, by using the macro `QCPC_DECL(name)`
- Definition, by using the macro `QCPC_DEF(name)`

You can combine them together by using the macro `QCPC_DECL_DEF(name)`. You
can not omit declarations.

Here is the grammar of a simple calculator:

```cpp
#include "qcpc/qcpc.hpp"

using namespace qcpc;

QCPC_DECL(expr);

QCPC_DECL_DEF_(sep)
  = *one<' ', '\t', '\r', '\n'>
  ;
QCPC_DECL_DEF(value)
  = +range<'0', '9'>
  | join(sep, one<'('>, expr, one<')'>)
  ;
QCPC_DECL_DEF(product_op)
  = one<'*'> | one<'/'>
  ;
QCPC_DECL_DEF(product)
  = list(value, product_op, sep)
  ;
QCPC_DECL_DEF(sum_op)
  = one<'+'> | one<'-'>
  ;
QCPC_DECL_DEF(sum)
  = list(product, sum_op, sep)
  ;
QCPC_DEF(expr)
  = sum
  ;
QCPC_DECL_DEF(grammar)
  = boi & expr & eoi
  ;
```

`QCPC_DECL(name)` will create an `inline constexpr` variable with anonymous
type, you can then use it in expressions and assign them to a `QCPC_DEF(name)`.

## Parsing

After defining your grammar, you can use it to parse strings. But before that,
you must construct an input object:

```cpp
StringInput in("(1+2)/3*5*6-2");
```

Now you can start parsing:

```cpp
auto ret = parse(expr, in);
```

The return type is `std::optional<qcpc::Token>`, `std::nullopt` means grammar
matches failed. `Token` is a tree-like object that stores the matching result.

## Processing

If match succeeds, you now have a `Token` object. Every user defined rules will
produce a `Token` object. If you do not expect that, you can use the silent
version `QCPC_DECL_` and `QCPC_DECL_DEF_`. There is no `QCPC_DEF_`.

Example:

```cpp
QCPC_DECL_DEF(two) = one<'2'>;
QCPC_DECL_DEF(twotwo) = two & two;
QCPC_DECL_DEF(foo) = one<'2'> & *twotwo;
```

Use `foo` to parse `"22222"` will produce:

```text
- foo
  - twotwo
    - two
    - two
  - twotwo
    - two
    - two
```

Every `Token` will have a tag. You can use tags to distinguish different
rules:

```cpp
switch (token.tag()) {
case value.tag: ...
case product.tag: ...
case sum.tag: ...
case expr.tag: ...
default: ...
}
```

To know more about `Token`s, please refer to the
[source code](/include/qcpc/comb/token.hpp).
