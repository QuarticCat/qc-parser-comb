#include <iostream>
#include <string_view>

#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

// clang-format off

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

// clang-format on

void print_tree(qcpc::Token& token, size_t indent = 0) {
    for (size_t i = 0; i < indent; ++i) putchar(' ');
    std::cout << token.view() << '\n';
    for (auto&& child: token.children) print_tree(child, indent + 2);
}

int eval(qcpc::Token& token) {
    switch (token.tag()) {
    case value.tag: {
        // std::cout << "value: " << token.view() << '\n';
        if (token.children.size() == 1) {
            int ret = 0;
            for (char c: token) ret = ret * 10 + (c - '0');
            return ret;
        } else {
            return eval(token.children[1]);
        }
    }
    case product.tag: {
        // std::cout << "product: " << token.view() << '\n';
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
        // std::cout << "sum: " << token.view() << '\n';
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
        // std::cout << "expr: " << token.view() << '\n';
        return eval(token.children[0]);
    }
    default: {
        // std::cout << "unknown!\n";
        return 0;
    }
    }
}

TEST(Calculator, Case1) {
    auto ret = qcpc::parse(expr, qcpc::StringInput("(1+2)/3*5*6-2"));
    // std::cout << "print tree:\n";
    // print_tree(*ret);
    // std::cout << "eval:\n";
    ASSERT_EQ(eval(*ret), 28);
}
