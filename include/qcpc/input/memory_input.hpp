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

struct InputPos {
    size_t line;
    size_t column;
    const char* current;
};

struct MemoryInput {
    MemoryInput(const char* begin, const char* end) noexcept: _current(begin), _end(end) {}

    INPUT_DECL_HELPER(MemoryInput);

    [[nodiscard]] const char& operator*() const noexcept {
        return *this->_current;
    }

    [[nodiscard]] bool is_eof() const noexcept {
        return this->_current == this->_end;
    }

    /// Return current position
    [[nodiscard]] InputPos pos() const noexcept {
        return {this->_line, this->_column, this->_current};
    }

    /// Jump to given position. Caller should ensure the correctness of the argument.
    void jump(InputPos pos) noexcept {
        this->_current = pos.current;
        this->_line = pos.line;
        this->_column = pos.column;
    }

  protected:
    size_t _line = 1;
    size_t _column = 0;
    const char* _current = nullptr;
    const char* _end = nullptr;

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
