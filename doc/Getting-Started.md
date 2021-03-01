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

Here is an example, the grammar of a simple calculator:

```cpp
#include "qcpc/qcpc.hpp"

using namespace qcpc;

QCPC_DECL(expr);

QCPC_DECL_DEF(value)
  = num
  | (one<'('> & expr & one<')'>)
  ;
QCPC_DECL_DEF(product)
  = value & *((one<'*'> | one<'/'>) & value)
  ;
QCPC_DECL_DEF(sum)
  = product & *((one<'+'> | one<'-'>) & product)
  ;
QCPC_DEF(expr)
  = sum
  ;
```

`QCPC_DECL(name)` will create an `inline constexpr` variable with anonymous
type, you can then use it in expressions and assign them to a `QCPC_DEF(name)`.

If you just want to create an alias, instead of a rule, you can simply assign
the expression to a `constexpr` variable:

```cpp
inline constexpr auto wrapped_expr = one<'('> & expr & one<')'>;
QCPC_DECL_DEF(value) = num | wrapped_expr;
```

Take it easy, all variables are empty and only for carrying type information,
assigning them to multiple rules or whatever will not bring any side-effects.

## Parsing

After defining your grammar, you can use it to parse strings. But before that,
you must construct an input object:

```cpp
StringInput in("(1+2)/3*5*6-2");
```

This library provide a sole entry `parse(rule, input)`. Now start parsing!

```cpp
auto ret = parse(expr, in);
```

The return type is `std::unique_ptr<qcpc::Token>`, `nullptr` means grammar
matches failed, `Token` is a tree-like object that stores the matching result.

## Processing

If match succeeds, you now have a `Token` object. Every atomic rules and user
defined rules will produce a `Token` object, while combinators does not. For
example, this grammar

```cpp
QCPC_DECL_DEF(foo) = one<'1'> & *(one<'2'> & one<'3'>);
```

parsing `"12323"` will produce

```text
- foo
  - one<'1'>
  - one<'2'>
  - one<'3'>
  - one<'2'>
  - one<'3'>
```

instead of

```text
- foo
  - seq
    - one<'1'>
    - star
      - seq
        - one<'2'>
        - one<'3'>
      - seq
        - one<'2'>
        - one<'3'>
```

Every `Token` produced by user defined rule will have a tag. You can use tags
to distinguish different rules:

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
