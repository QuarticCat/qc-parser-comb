#include <memory>

#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

TEST(TokenTest, IterToken) {
    const char cstr[] = "Handsome QC";
    qcpc::Token root({{cstr, 1, 0}, (&cstr)[1] - 1});
    ASSERT_EQ(root.line(), 1);
    ASSERT_EQ(root.column(), 0);
    size_t count = 0;
    for (auto c: root) ASSERT_EQ(c, cstr[count++]);
    ASSERT_EQ(count, sizeof(cstr) - 1);
}
