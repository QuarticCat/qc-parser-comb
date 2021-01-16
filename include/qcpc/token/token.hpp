#pragma once

#include <cstddef>
#include <memory>

namespace qcpc {

struct TokenPos {
    size_t line;
    size_t column;
    const char* begin;
    const char* end;
};

struct Token {
    struct Iter {
        // `Iter` must be defined inside `Token`. Because `Token` and `Iter` rely on each other,
        // both of them need the complete type of each other. Only inside classes or structs, C++
        // can solve signatures non-sequentially.

        explicit Iter(Token* node) noexcept: _node(node) {}

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
            this->_node = this->_node->_next.get();
            return *this;
        }

        [[nodiscard]] bool operator!=(Iter rhs) const noexcept {
            return this->_node != rhs._node;
        }

        [[nodiscard]] Token& operator*() const noexcept {
            return *this->_node;
        }

      private:
        Token* _node;
    };

    using Ptr = std::unique_ptr<Token>;  // save your keyboard and eyes :)

    Token() = default;  // TODO: do we really need this?

    explicit Token(TokenPos pos /* , rule */): _pos(pos) {}

    Token(const Token&) = delete;
    Token(Token&&) = default;
    Token& operator=(const Token&) = delete;
    Token& operator=(Token&&) = default;

    /// Return the pointer to the head child. Caller does not have its ownership.
    [[nodiscard]] Token* head_child() const noexcept {
        return this->_head_child.get();
    }

    /// Return the pointer to the next. Caller does not have its ownership.
    [[nodiscard]] Token* next() const noexcept {
        return this->_next.get();
    }

    // /// Return the position of this token.
    // [[nodiscard]] TokenPos pos() const noexcept {
    //     return this->_pos;
    // }

    /// Return line number of the first character
    [[nodiscard]] size_t line() const noexcept {
        return this->_pos.line;
    }

    /// Return column number of the first character
    [[nodiscard]] size_t column() const noexcept {
        return this->_pos.column;
    }

    [[nodiscard]] const char* begin() const noexcept {
        return this->_pos.begin;
    }

    [[nodiscard]] const char* end() const noexcept {
        return this->_pos.end;
    }

    /// Push a node to the front of the children list.
    void push_front(Ptr child) noexcept {
        child->_next = std::move(this->_head_child);
        this->_head_child = std::move(child);
    }

    /// Pop a node from the front of the children list. If there is no child, return null pointer.
    Ptr pop_front() noexcept {
        Ptr ret = std::move(this->_head_child);
        this->_head_child = std::move(ret->_next);
        return ret;
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
        return Iter(this->head_child());
    }

  private:
    // M-ary tree to binary tree:
    // Children are stored by a singly linked list.
    Ptr _head_child = nullptr;
    Ptr _next = nullptr;
    TokenPos _pos;
    // TODO: store rule
};

}  // namespace qcpc
