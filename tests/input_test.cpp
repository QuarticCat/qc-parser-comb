#include <string>

#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

TEST(InputTest, MemoryInput) {
    const char cstr[] = "012\n";
    qcpc::MemoryInput in(cstr, (&cstr)[1] - 1);
    ASSERT_EQ(*++in, '1');
    ASSERT_EQ(*++in, '2');
    ASSERT_EQ(*++in, '\n');
    ASSERT_EQ(in.pos().line, 1);
    ASSERT_EQ(in.pos().column, 3);
    ++in;
    ASSERT_EQ(in.pos().line, 2);
    ASSERT_EQ(in.pos().column, 0);
    ASSERT_TRUE(in.is_eof());
}

TEST(InputTest, StringInput) {
    qcpc::StringInput in("012\n");
    ASSERT_EQ(*++in, '1');
    ASSERT_EQ(*++in, '2');
    ASSERT_EQ(*++in, '\n');
    ASSERT_EQ(in.pos().line, 1);
    ASSERT_EQ(in.pos().column, 3);
    ++in;
    ASSERT_EQ(in.pos().line, 2);
    ASSERT_EQ(in.pos().column, 0);
    ASSERT_TRUE(in.is_eof());
}
