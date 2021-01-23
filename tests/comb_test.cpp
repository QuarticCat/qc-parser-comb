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

QCPC_DEFINE_RULE(bof_rule) = bof;

TEST(CombTest, RuleStruct_Bof) {
    StringInput in("1");
    ASSERT_TRUE(QCPC_PARSE(bof_rule, in));
    ++in;
    ASSERT_FALSE(QCPC_PARSE(bof_rule, in));
}

QCPC_DEFINE_RULE(eof_rule) = eof;

TEST(CombTest, RuleStruct_Eof) {
    StringInput in("1");
    ASSERT_FALSE(QCPC_PARSE(eof_rule, in));
    ++in;
    ASSERT_TRUE(QCPC_PARSE(eof_rule, in));
}

QCPC_DEFINE_RULE(str_rule) = str<'q', 'c', 'p', 'c'>;

TEST(CombTest, RuleStruct_Str) {
    StringInput in1("qcpc");
    ASSERT_TRUE(QCPC_PARSE(str_rule, in1));
    ASSERT_EQ(in1.current(), in1.end());

    StringInput in2("qcp");
    ASSERT_FALSE(QCPC_PARSE(str_rule, in2));
    ASSERT_EQ(in2.current(), in2.begin());
}

QCPC_DEFINE_RULE(at_rule) = +str_rule;

TEST(CombTest, RuleStruct_At) {
    StringInput in1("qcpc");
    ASSERT_TRUE(QCPC_PARSE(at_rule, in1));
    ASSERT_EQ(in1.current(), in1.begin());

    StringInput in2("qcp");
    ASSERT_FALSE(QCPC_PARSE(at_rule, in2));
    ASSERT_EQ(in2.current(), in2.begin());
}

QCPC_DEFINE_RULE(notat_rule) = -str_rule;

TEST(CombTest, RuleStruct_NotAt) {
    StringInput in1("qcpc");
    ASSERT_FALSE(QCPC_PARSE(notat_rule, in1));
    ASSERT_EQ(in1.current(), in1.begin());

    StringInput in2("qcp");
    ASSERT_TRUE(QCPC_PARSE(notat_rule, in2));
    ASSERT_EQ(in2.current(), in2.begin());
}

// TODO: need a better test case, e.g. silent seq
QCPC_DEFINE_RULE(silent_rule) = ~str_rule;

TEST(CombTest, RuleStruct_Silent) {
    StringInput in1("qcpc");
    Token::Ptr root1 = QCPC_PARSE(silent_rule, in1);
    ASSERT_EQ(in1.current(), in1.end());
    ASSERT_TRUE(root1->is_empty());

    StringInput in2("qcp");
    Token::Ptr root2 = QCPC_PARSE(silent_rule, in1);
    ASSERT_EQ(in2.current(), in2.begin());
}

}  // namespace rule_test
