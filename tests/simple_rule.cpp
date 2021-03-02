#include <concepts>

#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

using namespace qcpc;

QCPC_DECL_DEF(boi_rule) = boi;

TEST(SimpleRule, Boi) {
    StringInput in("1");
    ASSERT_TRUE(parse(boi_rule, in));
    ++in;
    ASSERT_FALSE(parse(boi_rule, in));
}

QCPC_DECL_DEF(eoi_rule) = eoi;

TEST(SimpleRule, Eoi) {
    StringInput in("1");
    ASSERT_FALSE(parse(eoi_rule, in));
    ++in;
    ASSERT_TRUE(parse(eoi_rule, in));
}

QCPC_DECL_DEF(bol_rule) = bol;

TEST(SimpleRule, Bol) {
    StringInput in("1\n2");
    ASSERT_TRUE(parse(bol_rule, in));
    ++in;
    ASSERT_FALSE(parse(bol_rule, in));
    ++in;
    ASSERT_TRUE(parse(bol_rule, in));
    ++in;
    ASSERT_FALSE(parse(bol_rule, in));
}

QCPC_DECL_DEF(eol_rule) = eol;

TEST(SimpleRule, Eol) {
    StringInput in1("\r\n");
    ASSERT_TRUE(parse(eol_rule, in1));
    ASSERT_EQ(in1.current(), in1.end());

    StringInput in2("\n");
    ASSERT_TRUE(parse(eol_rule, in2));
    ASSERT_EQ(in2.current(), in2.end());

    StringInput in3("\r");
    ASSERT_FALSE(parse(eol_rule, in3));
    ASSERT_EQ(in3.current(), in3.begin());
}

QCPC_DECL_DEF(one_rule) = one<'x'>;

TEST(SimpleRule, One) {
    StringInput in1("x");
    ASSERT_TRUE(parse(one_rule, in1));
    ASSERT_EQ(in1.current(), in1.end());

    StringInput in2("y");
    ASSERT_FALSE(parse(one_rule, in2));
    ASSERT_EQ(in2.current(), in2.begin());
}

// QCPC_DECL_DEF(str_rule) = str<'q', 'c', 'p', 'c'>;
//
// TEST(SimpleRule, Str) {
//     StringInput in1("qcpc");
//     ASSERT_TRUE(parse(str_rule, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//
//     StringInput in2("qcp");
//     ASSERT_FALSE(parse(str_rule, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
// }
//
// QCPC_DECL_DEF(range_rule) = range<'a', 'z', 'A', 'Z'>;
//
// TEST(SimpleRule, Range) {
//     StringInput in1("a");
//     ASSERT_TRUE(parse(range_rule, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//
//     StringInput in2("-");
//     ASSERT_FALSE(parse(range_rule, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
// }
//
// QCPC_DECL_DEF(alpha_rule) = alpha;
//
// TEST(SimpleRule, Alpha) {
//     StringInput in1("abc");
//     ASSERT_TRUE(parse(alpha_rule, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//
//     StringInput in2("---");
//     ASSERT_FALSE(parse(alpha_rule, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
//
//     StringInput in3("");
//     ASSERT_FALSE(parse(alpha_rule, in3));
//     ASSERT_EQ(in3.current(), in3.begin());
// }
//
// QCPC_DECL_DEF(num_rule) = num;
//
// TEST(SimpleRule, Num) {
//     StringInput in1("123");
//     ASSERT_TRUE(parse(num_rule, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//
//     StringInput in2("---");
//     ASSERT_FALSE(parse(num_rule, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
//
//     StringInput in3("");
//     ASSERT_FALSE(parse(num_rule, in3));
//     ASSERT_EQ(in3.current(), in3.begin());
// }
//
// QCPC_DECL_DEF(alnum_rule) = alnum;
//
// TEST(SimpleRule, AlNum) {
//     StringInput in1("1b3");
//     ASSERT_TRUE(parse(alnum_rule, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//
//     StringInput in2("---");
//     ASSERT_FALSE(parse(alnum_rule, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
//
//     StringInput in3("");
//     ASSERT_FALSE(parse(alnum_rule, in3));
//     ASSERT_EQ(in3.current(), in3.begin());
// }
//
// QCPC_DECL_DEF(blank_rule) = blank;
//
// TEST(SimpleRule, Blank) {
//     StringInput in1(" \t\r\n");
//     ASSERT_TRUE(parse(blank_rule, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//
//     StringInput in2("---");
//     ASSERT_FALSE(parse(blank_rule, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
//
//     StringInput in3("");
//     ASSERT_FALSE(parse(blank_rule, in3));
//     ASSERT_EQ(in3.current(), in3.begin());
// }
//
// QCPC_DECL_DEF(at_rule) = &str_rule;
//
// TEST(SimpleRule, At) {
//     StringInput in1("qcpc");
//     ASSERT_TRUE(parse(at_rule, in1));
//     ASSERT_EQ(in1.current(), in1.begin());
//
//     StringInput in2("qcp");
//     ASSERT_FALSE(parse(at_rule, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
// }
//
// QCPC_DECL_DEF(notat_rule) = !str_rule;
//
// TEST(SimpleRule, NotAt) {
//     StringInput in1("qcpc");
//     ASSERT_FALSE(parse(notat_rule, in1));
//     ASSERT_EQ(in1.current(), in1.begin());
//
//     StringInput in2("qcp");
//     ASSERT_TRUE(parse(notat_rule, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
// }
//
// QCPC_DECL_DEF(opt_rule) = -str_rule;
//
// TEST(SimpleRule, Opt) {
//     StringInput in1("qcpc");
//     ASSERT_TRUE(parse(opt_rule, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//
//     StringInput in2("qcp");
//     ASSERT_TRUE(parse(opt_rule, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
// }
//
// QCPC_DECL_DEF(star_rule) = *str_rule;
//
// TEST(SimpleRule, Star) {
//     StringInput in1("qcpc"
//                     "qcpc");
//     ASSERT_TRUE(parse(star_rule, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//
//     StringInput in2("qcp");
//     ASSERT_TRUE(parse(star_rule, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
// }
//
// QCPC_DECL_DEF(plus_rule) = +str_rule;
//
// TEST(SimpleRule, Plus) {
//     StringInput in1("qcpc"
//                     "qcpc");
//     ASSERT_TRUE(parse(plus_rule, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//
//     StringInput in2("qcp");
//     ASSERT_FALSE(parse(plus_rule, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
// }

#define COMPARE_RULE(rule1, rule2)                             \
    (std::same_as<decltype(detail::rule_set<decltype(rule1)>), \
                  decltype(detail::rule_set<decltype(rule2)>)>)

// QCPC_DECL_DEF(seq_rule1) = str_rule & str_rule & str_rule & str_rule;
// QCPC_DECL_DEF(seq_rule2) = (str_rule & str_rule) & str_rule & str_rule;
// QCPC_DECL_DEF(seq_rule3) = str_rule & str_rule & (str_rule & str_rule);
// QCPC_DECL_DEF(seq_rule4) = (str_rule & str_rule) & (str_rule & str_rule);
//
// TEST(SimpleRule, Seq) {
//     ASSERT_TRUE(COMPARE_RULE(seq_rule1, seq_rule2));
//     ASSERT_TRUE(COMPARE_RULE(seq_rule2, seq_rule3));
//     ASSERT_TRUE(COMPARE_RULE(seq_rule3, seq_rule4));
//
//     StringInput in1("qcpc"
//                     "qcpc"
//                     "qcpc"
//                     "qcpc");
//     InputPos pos1 = in1.pos();
//     ASSERT_TRUE(parse(seq_rule1, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//     in1.jump(pos1);
//     ASSERT_TRUE(parse(seq_rule2, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//     in1.jump(pos1);
//     ASSERT_TRUE(parse(seq_rule3, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//     in1.jump(pos1);
//     ASSERT_TRUE(parse(seq_rule4, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//
//     StringInput in2("qcpc");
//     ASSERT_FALSE(parse(seq_rule1, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
//     ASSERT_FALSE(parse(seq_rule2, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
//     ASSERT_FALSE(parse(seq_rule3, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
//     ASSERT_FALSE(parse(seq_rule4, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
// }
//
// QCPC_DECL_DEF(sor_rule1) = str<'a'> | str<'b'> | str<'c'> | str<'d'>;
// QCPC_DECL_DEF(sor_rule2) = (str<'a'> | str<'b'>) | str<'c'> | str<'d'>;
// QCPC_DECL_DEF(sor_rule3) = str<'a'> | str<'b'> | (str<'c'> | str<'d'>);
// QCPC_DECL_DEF(sor_rule4) = (str<'a'> | str<'b'>) | (str<'c'> | str<'d'>);
//
// TEST(SimpleRule, Sor) {
//     ASSERT_TRUE(COMPARE_RULE(sor_rule1, sor_rule2));
//     ASSERT_TRUE(COMPARE_RULE(sor_rule2, sor_rule3));
//     ASSERT_TRUE(COMPARE_RULE(sor_rule3, sor_rule4));
//
//     StringInput in1("a");
//     InputPos pos1 = in1.pos();
//     ASSERT_TRUE(parse(sor_rule1, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//     in1.jump(pos1);
//     ASSERT_TRUE(parse(sor_rule2, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//     in1.jump(pos1);
//     ASSERT_TRUE(parse(sor_rule3, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//     in1.jump(pos1);
//     ASSERT_TRUE(parse(sor_rule4, in1));
//     ASSERT_EQ(in1.current(), in1.end());
//
//     StringInput in2("e");
//     ASSERT_FALSE(parse(sor_rule1, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
//     ASSERT_FALSE(parse(sor_rule2, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
//     ASSERT_FALSE(parse(sor_rule3, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
//     ASSERT_FALSE(parse(sor_rule4, in2));
//     ASSERT_EQ(in2.current(), in2.begin());
// }
