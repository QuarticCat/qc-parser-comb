#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

using namespace qcpc;

QCPC_DECL_DEF(flatten1) = one<'1'> & *(one<'2'> & one<'3'>);
QCPC_DECL_DEF(flatten2) = one<'1'> & +(one<'2'> & one<'3'>);

TEST(CompundRuleTest, Flatten) {
    auto ret = parse(flatten1, StringInput("12323"));
    ASSERT_TRUE(ret);
    ASSERT_EQ(ret->children.size(), 5);

    ret = parse(flatten2, StringInput("12323"));
    ASSERT_TRUE(ret);
    ASSERT_EQ(ret->children.size(), 5);
}
