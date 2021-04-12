#pragma once

#include "header.hpp"

namespace qcpc {

/// Match the beginning of input. Consume nothing.
struct Boi {
    QCPC_DETAIL_DEFINE_PARSE(in, ) {
        return in.is_boi();
    }
};

inline constexpr Boi boi{};

/// Match the end of input. Consume nothing.
struct Eoi {
    QCPC_DETAIL_DEFINE_PARSE(in, ) {
        return in.is_eoi();
    }
};

inline constexpr Eoi eoi{};

/// Match the beginning of lines. Consume nothing.
struct Bol {
    QCPC_DETAIL_DEFINE_PARSE(in, ) {
        return in.column() == 0;
    }
};

inline constexpr Bol bol{};

/// Match the end of lines. Consume "\r\n" or "\n".
struct Eol {
    QCPC_DETAIL_DEFINE_PARSE(in, ) {
        auto pos = in.pos();
        if (*in == '\n') {
            ++in;
            return true;
        }
        if (*in == '\r') {
            if (*++in == '\n') {
                ++in;
                return true;
            }
            in.jump(pos);
        }
        return false;
    }
};

inline constexpr Eol eol{};

}  // namespace qcpc
