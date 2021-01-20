#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

TEST(CombTest, ParseRetToken) {
    auto ptr = new qcpc::Token({}, 0);
    qcpc::ParseRet res(ptr);
    ASSERT_TRUE(res.is_ptr());
    ASSERT_FALSE(res.is_result());
    ASSERT_EQ(res.get_ptr().get(), ptr);
}

TEST(CombTest, ParseRetResult) {
    qcpc::ParseRet res(true);
    ASSERT_FALSE(res.is_ptr());
    ASSERT_TRUE(res.is_result());
    ASSERT_EQ(res.get_result(), true);
}
