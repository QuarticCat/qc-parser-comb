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

TEST(TokenTest, PushIterPop) {
    qcpc::TokenPos pos({});
    qcpc::Token root(pos);
    root.push_front(std::make_unique<qcpc::Token>(pos));
    root.push_front(std::make_unique<qcpc::Token>(pos));
    root.push_front(std::make_unique<qcpc::Token>(pos));
    size_t count = 0;
    for ([[maybe_unused]] auto&& _: root.iter_children()) ++count;
    ASSERT_EQ(count, 3);
    for (size_t i = 0; i < count; ++i) root.pop_front();
    ASSERT_EQ(root.head_child(), nullptr);
    ASSERT_EQ(root.next(), nullptr);
}
