#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

TEST(CombTest, ParseRet_Token) {
    auto ptr = new qcpc::Token({}, 0);
    qcpc::ParseRet res(ptr);
    ASSERT_TRUE(res.is_ptr());
    ASSERT_FALSE(res.is_result());
    ASSERT_EQ(res.get_ptr().get(), ptr);
}

TEST(CombTest, ParseRet_Result) {
    qcpc::ParseRet res(true);
    ASSERT_FALSE(res.is_ptr());
    ASSERT_TRUE(res.is_result());
    ASSERT_EQ(res.get_result(), true);
}

namespace rule_test {

using namespace qcpc;

TEST(CombTest, RuleStruct_Str) {
    // TODO
}

}  // namespace rule_test
