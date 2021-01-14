#pragma once

#include <string>
#include <utility>

#include "memory_input.hpp"

namespace qcpc {

struct StringInput: MemoryInput {
    explicit StringInput(const std::string& str) noexcept
        : _str(str), MemoryInput(&*str.begin(), &*str.end()) {}

    explicit StringInput(std::string&& str) noexcept
        : _str(std::move(str)), MemoryInput(&*str.begin(), &*str.end()) {}

    INPUT_DECL_HELPER(StringInput);

  protected:
    std::string _str;
};

}  // namespace qcpc
