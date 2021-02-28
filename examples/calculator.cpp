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

int eval(qcpc::Token* ptr) {
    switch (ptr->tag()) {
    case value.tag: {
        std::cout << "value: " << ptr->view() << '\n';
        auto head = ptr->head();
        if (head->is_empty()) {
            int ret = 0;
            for (char c: *head) ret = ret * 10 + (c - '0');
            return ret;
        } else {
            return eval(head->head()->next());
        }
    }
    case product.tag: {
        std::cout << "product: " << ptr->view() << '\n';
        auto iter = ptr->head()->iter();
        int ret = eval(iter++);
        for (auto& seq: iter->iter()) {
            auto sub_iter = seq.iter();
            char op = *sub_iter++->begin();
            int rhs = eval(sub_iter);
            op == '*' ? ret *= rhs : ret /= rhs;
        }
        return ret;
    }
    case sum.tag: {
        std::cout << "sum: " << ptr->view() << '\n';
        auto iter = ptr->head()->iter();
        int ret = eval(iter++);
        for (auto& seq: iter->iter()) {
            auto sub_iter = seq.iter();
            char op = *sub_iter++->begin();
            int rhs = eval(sub_iter);
            op == '+' ? ret += rhs : ret -= rhs;
        }
        return ret;
    }
    case expr.tag: {
        std::cout << "expr: " << ptr->view() << '\n';
        return eval(ptr->head());
    }
    default: {
        std::cout << "unknown!\n";
        return 0;
    }
    }
}

TEST(CalculatorTest, Case1) {
    auto ret = qcpc::parse(expr, qcpc::StringInput("(1+2)/3*5*6-2"));
    ASSERT_EQ(eval(ret.get()), 28);
}
