#pragma once

#include <cstddef>
#include <memory>

#include "../token/token.hpp"

namespace qcpc {

/// Return value of rule type's `parse` member function.
///
/// Due to the "silent" feature, a parsing function can return two types of value. When it is
/// silent, only return matching result (bool). Otherwise it should return a `Token::Ptr`. This
/// class can store both of them.
struct ParseRet {
    // This class is something like the `DiscriminantPtr` in Facebook's folly project, but weaker
    // and more specialized. The key point is, currently on x86-64 platforms, the higher 12 bits of
    // pointers are not used. We can use them as flags. Besides, all major OS's are using the higher
    // half addresses as kernel space. Thus, we do not need to care about cases that higher 12 bits
    // are all ones.

    /// Construct from raw ptr. Then the `ParseRet` instance has the ownership of the object it
    /// points to. (non-silent cases)
    explicit constexpr ParseRet(Token* ptr) noexcept: _ptr(ptr) {}

    /// Construct from unique ptr. (non-silent cases)
    explicit ParseRet(Token::Ptr&& ptr) noexcept: _ptr(ptr.release()) {}

    /// Construct from a bool value indicating the matching result. (silent cases)
    explicit constexpr ParseRet(bool result) noexcept: _result(result ? _success : _fail) {}

    constexpr ParseRet(ParseRet&& other) noexcept: _ptr(other._ptr) {
        if (other.is_ptr()) other._ptr = nullptr;
    }

    constexpr ParseRet& operator=(ParseRet&& other) noexcept {
        this->_ptr = other._ptr;
        if (other.is_ptr()) other._ptr = nullptr;
        return *this;
    }

    ParseRet(const ParseRet&) = delete;
    ParseRet& operator=(const ParseRet&) = delete;

    /// Check if the inner type is ptr.
    [[nodiscard]] constexpr bool is_ptr() const noexcept {
        return !this->is_result();
    }

    /// Check if the inner type is result.
    [[nodiscard]] constexpr bool is_result() const noexcept {
        return this->_result & _flag;
    }

    /// Check if the match is successful.
    [[nodiscard]] constexpr bool success() const noexcept {
        return this->is_ptr() ? this->_ptr != nullptr : this->_result == _success;
    }

    /// Get the inner ptr and take ownership away. Caller should ensure the inner type is correct.
    Token::Ptr get_ptr() noexcept {
        Token* ret = this->_ptr;
        this->_ptr = nullptr;
        return Token::Ptr(ret);
    }

    /// Get the inner result. Caller should ensure the inner type is correct.
    [[nodiscard]] constexpr bool get_result() const noexcept {
        return this->_result == _success;
    }

    constexpr ~ParseRet() {
        if (this->is_ptr() && this->_ptr != nullptr) delete this->_ptr;
    }

  private:
    // C++20 guarantees that integers are represented in 2's completion form, thus these bitwise
    // operations are safe.

    /// MSB == 1 indicates that it is `_result` instead of `_ptr`.
    static constexpr size_t _flag = ~(-1ull >> 1);
    /// Second MSB == 1 indicates that the _result is true.
    static constexpr size_t _success = ~(-1ull >> 2);
    /// Second MSB == 0 indicates that the _result is false.
    static constexpr size_t _fail = ~(-1ull >> 1);

    union {
        Token* _ptr;
        size_t _result;
    };
};

}  // namespace qcpc
