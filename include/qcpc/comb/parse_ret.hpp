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
    // and more specialized. The key point is, currently on x86-64 platforms, the higher 16 bits of
    // pointers are not used. We can use them as flags.

    // TODO: Do we need to check the case that the higher 16 bits are all ones?

    /// Construct from raw ptr. Then the `ParseRet` instance has the ownership of the object it
    /// points to. (non-silent cases)
    explicit ParseRet(Token* ptr) noexcept: _ptr(ptr) {}

    /// Construct from unique ptr. (non-silent cases)
    explicit ParseRet(Token::Ptr&& ptr) noexcept: _ptr(ptr.release()) {}

    /// Construct from a bool value indicating the matching result. (silent cases)
    explicit ParseRet(bool result) noexcept: _result(result ? _success : _fail) {}

    ParseRet(ParseRet&& other) noexcept: _ptr(other._ptr) {
        if (other.is_ptr()) other._ptr = nullptr;
    }

    ParseRet& operator=(ParseRet&& other) noexcept {
        this->_ptr = other._ptr;
        if (other.is_ptr()) other._ptr = nullptr;
        return *this;
    }

    ParseRet(const ParseRet&) = delete;
    ParseRet& operator=(const ParseRet&) = delete;

    /// Check if the inner type is ptr.
    [[nodiscard]] bool is_ptr() const noexcept {
        return !this->is_result();
    }

    /// Check if the inner type is result.
    [[nodiscard]] bool is_result() const noexcept {
        return this->_result & _flag;
    }

    /// Get the inner ptr and take ownership away. Caller should ensure the inner type is correct.
    Token::Ptr get_ptr() noexcept {
        Token* ret = this->_ptr;
        this->_ptr = nullptr;
        return Token::Ptr(ret);
    }

    /// Get the inner result. Caller should ensure the inner type is correct.
    [[nodiscard]] bool get_result() const noexcept {
        return this->_result == _success;
    }

    ~ParseRet() {
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
