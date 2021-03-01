#pragma once

#include <string_view>
#include <vector>

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
    using Children = std::vector<Token>;

    Children children;

    explicit Token(TokenPos pos, RuleTag tag = NO_RULE): _pos(pos), _tag(tag) {}

    Token(Children children, TokenPos pos, RuleTag tag = NO_RULE)
        : children(std::move(children)), _pos(pos), _tag(tag) {}

    Token(const Token&) = delete;
    Token(Token&&) = default;
    Token& operator=(const Token&) = delete;
    Token& operator=(Token&&) = default;

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

  private:
    TokenPos _pos;
    RuleTag _tag;
};

}  // namespace qcpc
