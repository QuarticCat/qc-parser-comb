#pragma once

#include <concepts>
#include <cstddef>

namespace qcpc {

struct InputPos {
    const char* current;
    size_t line;
    size_t column;
};

// TODO: refactor this CRTP

template<class Derived>
struct InputCRTP {
    InputCRTP(const char* begin, const char* end) noexcept: _begin(begin), _end(end) {}

    InputCRTP(const InputCRTP&) = delete;
    InputCRTP(InputCRTP&&) = delete;
    InputCRTP& operator=(const InputCRTP&) = delete;
    InputCRTP& operator=(InputCRTP&&) = delete;

    Derived& operator++() noexcept {
        this->_next();
        return *static_cast<Derived*>(this);
    }

    [[nodiscard]] const char& operator*() const noexcept {
        return *this->_current;
    }

    [[nodiscard]] char operator[](size_t n) const noexcept {
        return this->_current[n];
    }

    [[nodiscard]] bool is_boi() const noexcept {
        return this->_current == this->_begin;
    }

    [[nodiscard]] bool is_eoi() const noexcept {
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

    /// Return current pointer.
    [[nodiscard]] const char* current() const noexcept {
        return this->_current;
    }

    /// Return current position.
    [[nodiscard]] InputPos pos() const noexcept {
        return {this->_current, this->_line, this->_column};
    }

    /// Return current line.
    [[nodiscard]] size_t line() const noexcept {
        return this->_line;
    }

    /// Return current column.
    [[nodiscard]] size_t column() const noexcept {
        return this->_column;
    }

    /// Jump to given position. Caller should ensure the correctness of the argument.
    void jump(InputPos pos) noexcept {
        this->_current = pos.current;
        this->_line = pos.line;
        this->_column = pos.column;
    }

    /// Execute self-increment `n` times.
    void advance(size_t n) noexcept {
        for (size_t i = 0; i < n; ++i) this->_next();
    }

  protected:
    const char* _begin = nullptr;
    const char* _end = nullptr;
    const char* _current = _begin;
    size_t _line = 1;
    size_t _column = 0;

    InputCRTP() = default;

    void _next() noexcept {
        if (*this->_current++ == '\n') {
            this->_line += 1;
            this->_column = 0;
        } else {
            this->_column += 1;
        }
    }
};

template<class T>
concept InputType = std::derived_from<T, InputCRTP<T>>;

}  // namespace qcpc
