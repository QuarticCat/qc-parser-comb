#pragma once

#include <cstddef>

#define INPUT_DECL_HELPER(type_name)                  \
    type_name(const type_name&) = default;            \
    type_name(type_name&&) = default;                 \
    type_name& operator=(const type_name&) = default; \
    type_name& operator=(type_name&&) = default;      \
    type_name& operator++() noexcept {                \
        this->_next();                                \
        return *this;                                 \
    }

namespace qcpc {

struct MemoryInput {
    MemoryInput(const char* begin, const char* end) noexcept
        : _begin(begin), _end(end), _line(0), _column(0) {}

    INPUT_DECL_HELPER(MemoryInput);

    [[nodiscard]] const char* begin() const noexcept {
        return this->_begin;
    }

    [[nodiscard]] const char* end() const noexcept {
        return this->_end;
    }

    [[nodiscard]] size_t line() const noexcept {
        return this->_line;
    }

    [[nodiscard]] size_t column() const noexcept {
        return this->_column;
    }

  protected:
    const char* _begin;
    const char* _end;
    size_t _line;
    size_t _column;

    void _next() noexcept {
        if (*this->_begin++ == '\n') {
            this->_line += 1;
            this->_column = 0;
        } else {
            this->_column += 1;
        }
    }
};

}  // namespace qcpc
