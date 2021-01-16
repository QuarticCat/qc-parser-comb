#include <memory>

#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

TEST(AstTest, ConstructAst) {
    qcpc::AstNode root{};
    root.push_front(std::make_unique<qcpc::AstNode>());
    root.push_front(std::make_unique<qcpc::AstNode>());
    root.push_front(std::make_unique<qcpc::AstNode>());
    size_t count = 0;
    for (auto&& node: root.iter_children()) ++count;
    ASSERT_EQ(count, 3);
}
