#include <iostream>
#include <string_view>

#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

using namespace qcpc;

// clang-format off

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

// clang-format on

int eval(qcpc::Token& token) {
    switch (token.tag()) {
    case value.tag: {
        std::cout << "value: " << token.view() << '\n';
        if (token.children.empty()) {
            int ret = 0;
            for (char c: token) ret = ret * 10 + (c - '0');
            return ret;
        } else {
            return eval(token.children[0]);
        }
    }
    case product.tag: {
        std::cout << "product: " << token.view() << '\n';
        auto iter = token.children.begin();
        int ret = eval(*iter++);
        while (iter != token.children.end()) {
            char op = *iter++->begin();
            int rhs = eval(*iter++);
            op == '*' ? ret *= rhs : ret /= rhs;
        }
        return ret;
    }
    case sum.tag: {
        std::cout << "sum: " << token.view() << '\n';
        auto iter = token.children.begin();
        int ret = eval(*iter++);
        while (iter != token.children.end()) {
            char op = *iter++->begin();
            int rhs = eval(*iter++);
            op == '+' ? ret += rhs : ret -= rhs;
        }
        return ret;
    }
    case expr.tag: {
        std::cout << "expr: " << token.view() << '\n';
        return eval(token.children[0]);
    }
    default: {
        std::cout << "unknown!\n";
        return 0;
    }
    }
}

TEST(Calculator, Case1) {
    StringInput in("(1+2)/3*5*6-2");
    auto ret = parse(expr, in);
    ASSERT_TRUE(ret);
    ASSERT_EQ(eval(ret->children[0]), 28);
}

TEST(Calculator, Case2) {
    StringInput in("( 1 + 2 ) / 3 * 5 * 6 - 2");
    auto ret = parse(expr, in);
    ASSERT_TRUE(ret);
    ASSERT_EQ(eval(ret->children[0]), 28);
}
