#pragma once

#include <cstddef>
#include <limits>
#include <memory>
#include <string_view>

#include "../input/input.hpp"
#include "rule_tag.hpp"  // do not include comb.hpp in case circular include

namespace qcpc {

struct TokenPos {
    const char* begin;
    const char* end;
    size_t line;
    size_t column;

    TokenPos(InputPos pos) noexcept
        : begin(pos.current), end(pos.current), line(pos.line), column(pos.column) {}

    TokenPos(InputPos start, const char* end) noexcept
        : begin(start.current), end(end), line(start.line), column(start.column) {}

    TokenPos(const TokenPos&) = default;
    TokenPos(TokenPos&&) = default;
    TokenPos& operator=(const TokenPos&) = default;
    TokenPos& operator=(TokenPos&&) = default;
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

        Iter operator++(int) noexcept {
            Iter before = *this;
            this->_node = this->_node->_next.get();
            return before;
        }

        operator Token*() noexcept {
            return this->_node;
        }

        [[nodiscard]] bool operator!=(Iter rhs) const noexcept {
            return this->_node != rhs._node;
        }

        [[nodiscard]] Token& operator*() const noexcept {
            return *this->_node;
        }

        [[nodiscard]] Token* operator->() const noexcept {
            return this->_node;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return this->_node != nullptr;
        }

      private:
        Token* _node;
    };

    using Ptr = std::unique_ptr<Token>;  // save your keyboard and eyes :)

    explicit Token(TokenPos pos, RuleTag tag = NO_RULE): _pos(pos), _tag(tag) {}

    Token(Token::Ptr children, TokenPos pos, RuleTag tag = NO_RULE)
        : _head(std::move(children)), _pos(pos), _tag(tag) {}

    Token(const Token&) = delete;
    Token(Token&&) = default;
    Token& operator=(const Token&) = delete;
    Token& operator=(Token&&) = default;

    /// Return the pointer to the head of the children list. Caller does not have its ownership.
    [[nodiscard]] Token* head() const noexcept {
        return this->_head.get();
    }

    /// Return the pointer to the next. Caller does not have its ownership.
    [[nodiscard]] Token* next() const noexcept {
        return this->_next.get();
    }

    /// Return the position of this token.
    [[nodiscard]] TokenPos pos() const noexcept {
        return this->_pos;
    }

    /// Return line number of the first character.
    [[nodiscard]] size_t line() const noexcept {
        return this->_pos.line;
    }

    /// Return column number of the first character.
    [[nodiscard]] size_t column() const noexcept {
        return this->_pos.column;
    }

    /// Return a pointer to the first character.
    [[nodiscard]] const char* begin() const noexcept {
        return this->_pos.begin;
    }

    /// Return a pointer to the tail character.
    [[nodiscard]] const char* end() const noexcept {
        return this->_pos.end;
    }

    [[nodiscard]] std::string_view view() const noexcept {
        return {this->begin(), this->end()};
    }

    /// Return tag of its rule.
    [[nodiscard]] RuleTag tag() const noexcept {
        return this->_tag;
    }

    /// Return whether it has children or not.
    [[nodiscard]] bool is_empty() const noexcept {
        return this->_head == nullptr;
    }

    /// Push a node to the front of the children list.
    void push_front(Ptr child) noexcept {
        child->_next = std::move(this->_head);
        this->_head = std::move(child);
    }

    /// Pop a node from the front of the children list. If there is no child, return null pointer.
    Ptr pop_front() noexcept {
        Ptr ret = std::move(this->_head);
        this->_head = std::move(ret->_next);
        return ret;
    }

    /// Link a node to its next.
    void link(Ptr next) noexcept {
        this->_next = std::move(next);
    }

    /// Return an iterator of the children list. It can also be used in range-based for loops.
    ///
    /// Usage:
    /// ```
    /// for (auto&& child: parent.iter()) {
    ///     // do something
    /// }
    /// ```
    [[nodiscard]] Iter iter() const noexcept {
        return Iter(this->head());
    }

  private:
    // M-ary tree to binary tree:
    // Children are stored by a singly linked list.
    Ptr _head = nullptr;
    Ptr _next = nullptr;
    TokenPos _pos;
    RuleTag _tag;
};

}  // namespace qcpc
