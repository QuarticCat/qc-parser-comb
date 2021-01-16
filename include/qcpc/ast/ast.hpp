#pragma once

#include <memory>

namespace qcpc {

struct AstNode {
    struct Iter {
        // `Iter` must be defined inside `AstNode`. Because `AstNode` and `Iter` relay on each
        // other, both of them need the complete type of each other. Only inside classes or
        // structs, C++ can solve signatures non-sequentially.

        explicit Iter(AstNode* node) noexcept: _node(node) {}

        Iter(const Iter&) = default;
        Iter(Iter&&) = default;
        Iter& operator=(const Iter&) = default;
        Iter& operator=(Iter&&) = default;

        [[nodiscard]] Iter begin() const noexcept {
            return *this;
        }

        [[nodiscard]] Iter end() const noexcept {
            return Iter(nullptr);
        }

        Iter& operator++() noexcept {
            this->_node = this->_node->next.get();
            return *this;
        }

        [[nodiscard]] bool operator!=(Iter rhs) const noexcept {
            return this->_node != rhs._node;
        }

        AstNode& operator*() const noexcept {
            return *this->_node;
        }

      private:
        AstNode* _node;
    };

    using NodePtr = std::unique_ptr<AstNode>;

    // M-ary tree to binary tree:
    // Children are stored by a singly linked list.
    NodePtr child_head = nullptr;
    NodePtr next = nullptr;

    AstNode() = default;
    AstNode(const AstNode&) = delete;
    AstNode(AstNode&&) = default;
    AstNode& operator=(const AstNode&) = delete;
    AstNode& operator=(AstNode&&) = default;

    /// Push a node to the front of the children list.
    void push_front(NodePtr child) noexcept {
        child->next = std::move(this->child_head);
        this->child_head = std::move(child);
    }

    /// Return an iterator of the children list. It can also be used in range-based for loops.
    ///
    /// Usage:
    /// ```
    /// for (auto&& child: parent.iter_children()) {
    ///     // do something
    /// }
    /// ```
    [[nodiscard]] Iter iter_children() const noexcept {
        return Iter(this->child_head.get());
    }
};

}  // namespace qcpc
