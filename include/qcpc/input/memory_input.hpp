#pragma once

#include "input_utils.hpp"

namespace qcpc {

struct MemoryInput: InputCRTP<MemoryInput> {
    MemoryInput(const char* begin, const char* end) noexcept: InputCRTP<MemoryInput>(begin, end) {}
};

}  // namespace qcpc
