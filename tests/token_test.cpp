#include <memory>

#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

TEST(TokenTest, IterToken) {
    const char cstr[] = "Handsome QC";
    qcpc::TokenPos pos{cstr, (&cstr)[1] - 1, 0, 0};
    qcpc::Token root(pos);
    size_t count = 0;
    for (auto c: root) ASSERT_EQ(c, cstr[count++]);
    ASSERT_EQ(count, sizeof(cstr) - 1);
}

TEST(TokenTest, PushIterPop) {
    qcpc::Token root{};
    root.push_front(std::make_unique<qcpc::Token>());
    root.push_front(std::make_unique<qcpc::Token>());
    root.push_front(std::make_unique<qcpc::Token>());
    size_t count = 0;
    for (auto&& node: root.iter_children()) ++count;
    ASSERT_EQ(count, 3);
    for (size_t i = 0; i < count; ++i) root.pop_front();
    ASSERT_EQ(root.head_child(), nullptr);
    ASSERT_EQ(root.next(), nullptr);
}
