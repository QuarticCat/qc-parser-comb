#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

using namespace qcpc;

QCPC_DECL_DEF(two) = one<'2'>;

QCPC_DECL_DEF(flatten1) = two & *(one<'1'> & two & one<'3'>);
QCPC_DECL_DEF(flatten2) = two & +(one<'1'> & two & one<'3'>);

TEST(CompundRule, Flatten) {
    auto ret = parse(flatten1, StringInput("2123123"));
    ASSERT_TRUE(ret);
    ASSERT_EQ(ret->children.size(), 3);

    ret = parse(flatten2, StringInput("2123123"));
    ASSERT_TRUE(ret);
    ASSERT_EQ(ret->children.size(), 3);
}

QCPC_DECL_DEF(unwind) = (two & one<'3'>) | (two & one<'4'>);

TEST(CompundRule, Unwind) {
    auto ret = parse(unwind, StringInput("24"));
    ASSERT_TRUE(ret);
    ASSERT_EQ(ret->children.size(), 1);
}

QCPC_DECL_DEF_(silent_one) = one<'1'>;
QCPC_SET_SEP(silent_one);
QCPC_DECL_DEF(separate) = two & two && two && two & two;

TEST(CompoundRule, Separate) {
    auto ret = parse(separate, StringInput("2212122"));
    ASSERT_TRUE(ret);
    ASSERT_EQ(ret->children.size(), 5);
}
