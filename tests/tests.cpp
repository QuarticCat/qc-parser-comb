#include <string>

#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

TEST(CompileTest, Main) {}

TEST(InputTest, StringInput) {
    qcpc::StringInput in(std::string("012\n"));
    ASSERT_EQ(*++in, '1');
    ASSERT_EQ(*++in, '2');
    ASSERT_EQ(*++in, '\n');
    ASSERT_EQ(in.line(), 1);
    ASSERT_EQ(in.column(), 3);
    ++in;
    ASSERT_EQ(in.line(), 2);
    ASSERT_EQ(in.column(), 0);
    ASSERT_TRUE(in.is_eof());
}
