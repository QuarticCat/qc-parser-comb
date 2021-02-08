#include <concepts>

#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

TEST(CombTest, ParseRet_Token_Success) {
    auto ptr = new qcpc::Token(qcpc::TokenPos({}), 0);
    qcpc::ParseRet res(ptr);
    ASSERT_TRUE(res.is_ptr());
    ASSERT_FALSE(res.is_result());
    ASSERT_TRUE(res.success());
    ASSERT_EQ(res.get_ptr().get(), ptr);
}

TEST(CombTest, ParseRet_Token_Fail) {
    qcpc::ParseRet res(nullptr);
    ASSERT_TRUE(res.is_ptr());
    ASSERT_FALSE(res.is_result());
    ASSERT_FALSE(res.success());
    ASSERT_EQ(res.get_ptr().get(), nullptr);
}

TEST(CombTest, ParseRet_Result_Success) {
    qcpc::ParseRet res(true);
    ASSERT_FALSE(res.is_ptr());
    ASSERT_TRUE(res.is_result());
    ASSERT_TRUE(res.success());
    ASSERT_EQ(res.get_result(), true);
}

TEST(CombTest, ParseRet_Result_Fail) {
    qcpc::ParseRet res(false);
    ASSERT_FALSE(res.is_ptr());
    ASSERT_TRUE(res.is_result());
    ASSERT_FALSE(res.success());
    ASSERT_EQ(res.get_result(), false);
}

namespace rule_test {

using namespace qcpc;

QCPC_DEFINE_RULE(boi_rule) = boi;

TEST(CombTest, Rule_Boi) {
    StringInput in("1");
    ASSERT_TRUE(parse(boi_rule, in));
    ++in;
    ASSERT_FALSE(parse(boi_rule, in));
}

QCPC_DEFINE_RULE(eoi_rule) = eoi;

TEST(CombTest, Rule_Eoi) {
    StringInput in("1");
    ASSERT_FALSE(parse(eoi_rule, in));
    ++in;
    ASSERT_TRUE(parse(eoi_rule, in));
}

QCPC_DEFINE_RULE(bol_rule) = bol;

TEST(CombTest, Rule_Bol) {
    StringInput in("1\n2");
    ASSERT_TRUE(parse(bol_rule, in));
    ++in;
    ASSERT_FALSE(parse(bol_rule, in));
    ++in;
    ASSERT_TRUE(parse(bol_rule, in));
    ++in;
    ASSERT_FALSE(parse(bol_rule, in));
}

QCPC_DEFINE_RULE(eol_rule) = eol;

TEST(CombTest, Rule_Eol) {
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

QCPC_DEFINE_RULE(str_rule) = str<'q', 'c', 'p', 'c'>;

TEST(CombTest, Rule_Str) {
    StringInput in1("qcpc");
    ASSERT_TRUE(parse(str_rule, in1));
    ASSERT_EQ(in1.current(), in1.end());

    StringInput in2("qcp");
    ASSERT_FALSE(parse(str_rule, in2));
    ASSERT_EQ(in2.current(), in2.begin());
}

QCPC_DEFINE_RULE(at_rule) = &str_rule;

TEST(CombTest, Rule_At) {
    StringInput in1("qcpc");
    ASSERT_TRUE(parse(at_rule, in1));
    ASSERT_EQ(in1.current(), in1.begin());

    StringInput in2("qcp");
    ASSERT_FALSE(parse(at_rule, in2));
    ASSERT_EQ(in2.current(), in2.begin());
}

QCPC_DEFINE_RULE(notat_rule) = !str_rule;

TEST(CombTest, Rule_NotAt) {
    StringInput in1("qcpc");
    ASSERT_FALSE(parse(notat_rule, in1));
    ASSERT_EQ(in1.current(), in1.begin());

    StringInput in2("qcp");
    ASSERT_TRUE(parse(notat_rule, in2));
    ASSERT_EQ(in2.current(), in2.begin());
}

QCPC_DEFINE_RULE(opt_rule) = -str_rule;

TEST(CombTest, Rule_Opt) {
    StringInput in1("qcpc");
    ASSERT_TRUE(parse(opt_rule, in1));
    ASSERT_EQ(in1.current(), in1.end());

    StringInput in2("qcp");
    ASSERT_TRUE(parse(opt_rule, in2));
    ASSERT_EQ(in2.current(), in2.begin());
}

QCPC_DEFINE_RULE(star_rule) = *str_rule;

TEST(CombTest, Rule_Star) {
    StringInput in1("qcpc"
                    "qcpc");
    ASSERT_TRUE(parse(star_rule, in1));
    ASSERT_EQ(in1.current(), in1.end());

    StringInput in2("qcp");
    ASSERT_TRUE(parse(star_rule, in2));
    ASSERT_EQ(in2.current(), in2.begin());
}

QCPC_DEFINE_RULE(plus_rule) = +str_rule;

TEST(CombTest, Rule_Plus) {
    StringInput in1("qcpc"
                    "qcpc");
    ASSERT_TRUE(parse(plus_rule, in1));
    ASSERT_EQ(in1.current(), in1.end());

    StringInput in2("qcp");
    ASSERT_FALSE(parse(plus_rule, in2));
    ASSERT_EQ(in2.current(), in2.begin());
}

QCPC_DEFINE_RULE(silent_rule) = ~str_rule;

TEST(CombTest, Rule_Silent) {
    StringInput in1("qcpc");
    Token::Ptr root1 = parse(silent_rule, in1);
    ASSERT_EQ(in1.current(), in1.end());
    ASSERT_TRUE(root1->is_empty());

    StringInput in2("qcp");
    Token::Ptr root2 = parse(silent_rule, in1);
    ASSERT_EQ(in2.current(), in2.begin());
}

QCPC_DEFINE_RULE(seq_rule1) = str_rule & str_rule & str_rule & str_rule;
QCPC_DEFINE_RULE(seq_rule1s) = ~(str_rule & str_rule & str_rule & str_rule);
QCPC_DEFINE_RULE(seq_rule2) = (str_rule & str_rule) & str_rule & str_rule;
QCPC_DEFINE_RULE(seq_rule3) = str_rule & str_rule & (str_rule & str_rule);
QCPC_DEFINE_RULE(seq_rule4) = (str_rule & str_rule) & (str_rule & str_rule);

TEST(CombTest, Rule_Seq) {
    ASSERT_TRUE((std::same_as<decltype(seq_rule1)::Rule, decltype(seq_rule2)::Rule>));
    ASSERT_TRUE((std::same_as<decltype(seq_rule2)::Rule, decltype(seq_rule3)::Rule>));
    ASSERT_TRUE((std::same_as<decltype(seq_rule3)::Rule, decltype(seq_rule4)::Rule>));

    StringInput in1("qcpc"
                    "qcpc"
                    "qcpc"
                    "qcpc");
    InputPos pos1 = in1.pos();
    ASSERT_TRUE(parse(seq_rule1, in1));
    ASSERT_EQ(in1.current(), in1.end());
    in1.jump(pos1);
    ASSERT_TRUE(parse(seq_rule1s, in1));
    ASSERT_EQ(in1.current(), in1.end());
    in1.jump(pos1);
    ASSERT_TRUE(parse(seq_rule2, in1));
    ASSERT_EQ(in1.current(), in1.end());
    in1.jump(pos1);
    ASSERT_TRUE(parse(seq_rule3, in1));
    ASSERT_EQ(in1.current(), in1.end());
    in1.jump(pos1);
    ASSERT_TRUE(parse(seq_rule4, in1));
    ASSERT_EQ(in1.current(), in1.end());

    StringInput in2("qcpc");
    ASSERT_FALSE(parse(seq_rule1, in2));
    ASSERT_EQ(in2.current(), in2.begin());
    ASSERT_FALSE(parse(seq_rule1s, in2));
    ASSERT_EQ(in2.current(), in2.begin());
    ASSERT_FALSE(parse(seq_rule2, in2));
    ASSERT_EQ(in2.current(), in2.begin());
    ASSERT_FALSE(parse(seq_rule3, in2));
    ASSERT_EQ(in2.current(), in2.begin());
    ASSERT_FALSE(parse(seq_rule4, in2));
    ASSERT_EQ(in2.current(), in2.begin());
}

QCPC_DEFINE_RULE(sor_rule1) = str<'a'> | str<'b'> | str<'c'> | str<'d'>;
QCPC_DEFINE_RULE(sor_rule1s) = ~(str<'a'> | str<'b'> | str<'c'> | str<'d'>);
QCPC_DEFINE_RULE(sor_rule2) = (str<'a'> | str<'b'>) | str<'c'> | str<'d'>;
QCPC_DEFINE_RULE(sor_rule3) = str<'a'> | str<'b'> | (str<'c'> | str<'d'>);
QCPC_DEFINE_RULE(sor_rule4) = (str<'a'> | str<'b'>) | (str<'c'> | str<'d'>);

TEST(CombTest, Rule_Sor) {
    ASSERT_TRUE((std::same_as<decltype(sor_rule1)::Rule, decltype(sor_rule2)::Rule>));
    ASSERT_TRUE((std::same_as<decltype(sor_rule2)::Rule, decltype(sor_rule3)::Rule>));
    ASSERT_TRUE((std::same_as<decltype(sor_rule3)::Rule, decltype(sor_rule4)::Rule>));

    StringInput in1("a");
    InputPos pos1 = in1.pos();
    ASSERT_TRUE(parse(sor_rule1, in1));
    ASSERT_EQ(in1.current(), in1.end());
    in1.jump(pos1);
    ASSERT_TRUE(parse(sor_rule1s, in1));
    ASSERT_EQ(in1.current(), in1.end());
    in1.jump(pos1);
    ASSERT_TRUE(parse(sor_rule2, in1));
    ASSERT_EQ(in1.current(), in1.end());
    in1.jump(pos1);
    ASSERT_TRUE(parse(sor_rule3, in1));
    ASSERT_EQ(in1.current(), in1.end());
    in1.jump(pos1);
    ASSERT_TRUE(parse(sor_rule4, in1));
    ASSERT_EQ(in1.current(), in1.end());

    StringInput in2("e");
    ASSERT_FALSE(parse(sor_rule1, in2));
    ASSERT_EQ(in2.current(), in2.begin());
    ASSERT_FALSE(parse(sor_rule1s, in2));
    ASSERT_EQ(in2.current(), in2.begin());
    ASSERT_FALSE(parse(sor_rule2, in2));
    ASSERT_EQ(in2.current(), in2.begin());
    ASSERT_FALSE(parse(sor_rule3, in2));
    ASSERT_EQ(in2.current(), in2.begin());
    ASSERT_FALSE(parse(sor_rule4, in2));
    ASSERT_EQ(in2.current(), in2.begin());
}

}  // namespace rule_test
