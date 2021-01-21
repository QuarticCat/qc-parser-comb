#pragma once

#include <cstddef>

#define QCPC_INPUT_DECL_HELPER(type_name)            \
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
    const char* current;
    size_t line;
    size_t column;
};

struct MemoryInput {
    MemoryInput(const char* begin, const char* end) noexcept: _begin(begin), _end(end) {}

    QCPC_INPUT_DECL_HELPER(MemoryInput);

    [[nodiscard]] const char& operator*() const noexcept {
        return *this->_current;
    }

    [[nodiscard]] bool is_bof() const noexcept {
        return this->_current == this->_begin;
    }

    [[nodiscard]] bool is_eof() const noexcept {
        return this->_current == this->_end;
    }

    [[nodiscard]] const char* begin() const noexcept {
        return this->_begin;
    }

    [[nodiscard]] const char* end() const noexcept {
        return this->_end;
    }

    /// Return number of remaining characters.
    [[nodiscard]] size_t size() const noexcept {
        return this->_end - this->_current;
    }

    /// Return current pointer
    [[nodiscard]] const char* current() const noexcept {
        return this->_current;
    }

    /// Return current position
    [[nodiscard]] InputPos pos() const noexcept {
        return {this->_current, this->_line, this->_column};
    }

    /// Jump to given position. Caller should ensure the correctness of the argument.
    void jump(InputPos pos) noexcept {
        this->_current = pos.current;
        this->_line = pos.line;
        this->_column = pos.column;
    }

  protected:
    const char* _begin = nullptr;
    const char* _end = nullptr;
    const char* _current = _begin;
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
