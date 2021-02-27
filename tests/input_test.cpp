#include <string>

#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

constexpr const char test_cstr[] = "012\n";

template<typename Input>
void test_helper(Input& in, bool jump = false) {
    ASSERT_EQ(in.size(), sizeof(test_cstr) - 1);
    ASSERT_TRUE(in.is_boi());

    ASSERT_EQ(*++in, '1');
    ASSERT_EQ(*++in, '2');
    ASSERT_EQ(*++in, '\n');
    ASSERT_EQ(in.line(), 1);
    ASSERT_EQ(in.column(), 3);
    ++in;
    ASSERT_EQ(in.line(), 2);
    ASSERT_EQ(in.column(), 0);

    ASSERT_EQ(in.size(), 0);
    ASSERT_TRUE(in.is_eoi());

    if (jump) {
        in.jump({in.begin(), 1, 0});
        test_helper(in, false);
    }
}

TEST(InputTest, MemoryInput) {
    qcpc::MemoryInput in(test_cstr, (&test_cstr)[1] - 1);
    ASSERT_EQ(in.current(), test_cstr);
    test_helper(in);
    ASSERT_EQ(in.current(), (&test_cstr)[1] - 1);
}

TEST(InputTest, StringInput) {
    qcpc::StringInput in(test_cstr);
    test_helper(in);
}
