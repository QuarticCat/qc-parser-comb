#pragma once

#include <string>
#include <utility>

#include "memory_input.hpp"

namespace qcpc {

struct StringInput: MemoryInput {
    explicit StringInput(std::string str) noexcept: MemoryInput(), _str(std::move(str)) {
        // `str` should be moved to `_str` first, otherwise `_current` and `_end` may point to wrong
        // position due to SSO.
        // The self-references (`_current` and `_end` point to the data owned by `_str`) are safe,
        // because input classes are non-copyable and non-movable.
        this->_begin = &*this->_str.begin();
        this->_end = &*this->_str.end();
        this->_current = this->_begin;
    }

    QCPC_INPUT_DECL_HELPER(StringInput);

  protected:
    std::string _str;
};

}  // namespace qcpc
