#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

using namespace qcpc;

QCPC_DECL_DEF(two) = one<'2'>;

QCPC_DECL_DEF(flatten1) = two & *(one<'1'> & two & one<'3'>);
QCPC_DECL_DEF(flatten2) = two & +(one<'1'> & two & one<'3'>);

TEST(CompoundRule, Flatten) {
    StringInput in1("2123123");
    auto ret = parse(flatten1, in1);
    ASSERT_TRUE(ret);
    ASSERT_EQ(ret->children.size(), 3);

    StringInput in2("2123123");
    ret = parse(flatten2, in2);
    ASSERT_TRUE(ret);
    ASSERT_EQ(ret->children.size(), 3);
}

QCPC_DECL_DEF(unwind) = (two & one<'3'>) | (two & one<'4'>);

TEST(CompoundRule, Unwind) {
    StringInput in("24");
    auto ret = parse(unwind, in);
    ASSERT_TRUE(ret);
    ASSERT_EQ(ret->children.size(), 1);
}

QCPC_DECL_DEF_(silent_one) = one<'1'>;
QCPC_SET_SEP(silent_one);
QCPC_DECL_DEF(separate) = two & two && two && two & two;

TEST(CompoundRule, Separate) {
    StringInput in("2212122");
    auto ret = parse(separate, in);
    ASSERT_TRUE(ret);
    ASSERT_EQ(ret->children.size(), 5);
}
