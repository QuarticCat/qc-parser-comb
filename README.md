# QC Parser Comb

![c++20](https://img.shields.io/badge/c%2B%2B-20-blue)
![header-only](https://img.shields.io/badge/header-only-blue)

A header-only parser combinator library for fun. Aiming to be simple and elegant.

```cpp
QCPC_DECL(expr);

QCPC_DECL_DEF_(sep)
  = *one<' ', '\t', '\r', '\n'>
  ;
QCPC_DECL_DEF(value)
  = +range<'0', '9'>
  | join(sep, one<'('>, expr, one<')'>)
  ;
QCPC_DECL_DEF(product_op)
  = one<'*', '/'>
  ;
QCPC_DECL_DEF(product)
  = list(value, product_op, sep)
  ;
QCPC_DECL_DEF(sum_op)
  = one<'+', '-'>
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

## Examples

- [examples](/examples) folder
- [micro-lang](https://github.com/QuarticCat/micro-lang)

## Documentation

See [doc](/doc) folder.

## License

MIT

## Credits

This project was inspired by following excellent works:

- [pest](https://github.com/pest-parser/pest)
- [PEGTL](https://github.com/taocpp/PEGTL)
- [CppCmb](https://github.com/LPeter1997/CppCmb)
