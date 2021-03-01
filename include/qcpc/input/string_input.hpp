#pragma once

#include <string>
#include <utility>

#include "input_utils.hpp"

namespace qcpc {

struct StringInput: InputCRTP<StringInput> {
    explicit StringInput(std::string str) noexcept: InputCRTP<StringInput>(), _str(std::move(str)) {
        // `str` should be moved to `_str` first, otherwise `_current` and `_end` may point to wrong
        // position due to SSO.
        // The self-references (`_current` and `_end` point to the data owned by `_str`) are safe,
        // because input classes are non-copyable and non-movable.
        this->_begin = &*this->_str.begin();
        this->_end = &*this->_str.end();
        this->_current = this->_begin;
    }

  protected:
    std::string _str;
};

}  // namespace qcpc
