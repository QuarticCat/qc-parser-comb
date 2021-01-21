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
    ASSERT_TRUE(QCPC_PARSE(str_rule, StringInput("qcpc")));
    ASSERT_FALSE(QCPC_PARSE(str_rule, StringInput("qcp")));
}

}  // namespace rule_test
