#include <memory>

#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

TEST(TokenTest, IterToken) {
    const char cstr[] = "Handsome QC";
    qcpc::TokenPos pos{cstr, (&cstr)[1] - 1, 1, 0};
    qcpc::Token root(pos, 114514);
    ASSERT_EQ(root.line(), 1);
    ASSERT_EQ(root.column(), 0);
    ASSERT_EQ(root.rule(), 114514);
    size_t count = 0;
    for (auto c: root) ASSERT_EQ(c, cstr[count++]);
    ASSERT_EQ(count, sizeof(cstr) - 1);
}

TEST(TokenTest, PushIterPop) {
    qcpc::TokenPos pos{};
    qcpc::Token root(pos, 0);
    root.push_front(std::make_unique<qcpc::Token>(pos, 0));
    root.push_front(std::make_unique<qcpc::Token>(pos, 0));
    root.push_front(std::make_unique<qcpc::Token>(pos, 0));
    size_t count = 0;
    for (auto&& node: root.iter_children()) ++count;
    ASSERT_EQ(count, 3);
    for (size_t i = 0; i < count; ++i) root.pop_front();
    ASSERT_EQ(root.head_child(), nullptr);
    ASSERT_EQ(root.next(), nullptr);
}
