#pragma once

#include <cstddef>

#define INPUT_DECL_HELPER(type_name)                 \
    type_name(const type_name&) = delete;            \
    type_name(type_name&&) = delete;                 \
    type_name& operator=(const type_name&) = delete; \
    type_name& operator=(type_name&&) = delete;      \
    type_name& operator++() noexcept {               \
        this->_next();                               \
        return *this;                                \
    }

namespace qcpc {

struct Position {
    const char* current;
    size_t line;
    size_t column;
};

struct MemoryInput {
    MemoryInput(const char* begin, const char* end) noexcept: _current(begin), _end(end) {}

    INPUT_DECL_HELPER(MemoryInput);

    const char& operator*() const noexcept {
        return *this->_current;
    }

    [[nodiscard]] bool is_eof() const noexcept {
        return this->_current == this->_end;
    }

    /// Return current position
    [[nodiscard]] Position pos() const noexcept {
        return {this->_current, this->_line, this->_column};
    }

    /// Jump to given position. Caller should ensure the correctness of the argument.
    void jump(Position pos) noexcept {
        this->_current = pos.current;
        this->_line = pos.line;
        this->_column = pos.column;
    }

  protected:
    const char* _current = nullptr;
    const char* _end = nullptr;
    size_t _line = 1;
    size_t _column = 0;

    MemoryInput() = default;

    void _next() noexcept {
        if (*this->_current++ == '\n') {
            this->_line += 1;
            this->_column = 0;
        } else {
            this->_column += 1;
        }
    }
};

}  // namespace qcpc
