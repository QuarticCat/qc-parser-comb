#pragma once

#include "header.hpp"

namespace qcpc {

/// PEG and-predicate `&e`.
template<RuleType R>
struct At {
    QCPC_DETAIL_DEFINE_PARSE(At) {
        auto pos = in.pos();
        auto ret = R::parse(in, out, mem);
        in.jump(pos);
        return ret;
    }
};

template<RuleType R>
[[nodiscard]] constexpr At<R> operator&(R) {
    return {};
}

/// PEG not-predicate `!e`.
template<RuleType R>
struct NotAt {
    QCPC_DETAIL_DEFINE_PARSE(NotAt) {
        auto pos = in.pos();
        auto ret = R::parse(in, out, mem);
        in.jump(pos);
        return !ret;
    }
};

template<RuleType R>
[[nodiscard]] constexpr NotAt<R> operator!(R) {
    return {};
}

/// PEG optional `e?`.
template<RuleType R>
struct Opt {
    QCPC_DETAIL_DEFINE_PARSE(Opt) {
        R::parse(in, out, mem);
        return true;
    }
};

template<RuleType R>
[[nodiscard]] constexpr Opt<R> operator-(R) {
    return {};
}

/// PEG zero-or-more `e*`.
template<RuleType R>
struct Star {
    QCPC_DETAIL_DEFINE_PARSE(Star) {
        while (R::parse(in, out, mem)) {}
        return true;
    }
};

template<RuleType R>
[[nodiscard]] constexpr Star<R> operator*(R) {
    return {};
}

/// PEG one-or-more `e+`.
template<RuleType R>
struct Plus {
    QCPC_DETAIL_DEFINE_PARSE(Plus) {
        if (!R::parse(in, out, mem)) return false;
        while (R::parse(in, out, mem)) {}
        return true;
    }
};

template<RuleType R>
[[nodiscard]] constexpr Plus<R> operator+(R) {
    return {};
}

/// PEG sequence `e1 e2`.
template<RuleType... Rs>
struct Seq {
    QCPC_DETAIL_DEFINE_PARSE(Seq) {
        auto pos = in.pos();
        size_t size = out.size();
        if ((Rs::parse(in, out, mem) && ...)) return true;
        in.jump(pos);
        out.erase(out.begin() + size, out.end());
        return false;
    }
};

template<RuleType R1, RuleType R2>
[[nodiscard]] constexpr Seq<R1, R2> operator&(R1, R2) {
    return {};
}

template<RuleType R1, RuleType... R2s>
[[nodiscard]] constexpr Seq<R1, R2s...> operator&(R1, Seq<R2s...>) {
    return {};
}

template<RuleType... R1s, RuleType R2>
[[nodiscard]] constexpr Seq<R1s..., R2> operator&(Seq<R1s...>, R2) {
    return {};
}

template<RuleType... R1s, RuleType... R2s>
[[nodiscard]] constexpr Seq<R1s..., R2s...> operator&(Seq<R1s...>, Seq<R2s...>) {
    return {};
}

/// PEG ordered choice `e1 | e2`.
template<RuleType... Rs>
struct Sor {
    QCPC_DETAIL_DEFINE_PARSE(Sor) {
        if ((Rs::parse(in, out, mem) || ...)) return true;
        return false;
    }
};

template<RuleType R1, RuleType R2>
[[nodiscard]] constexpr Sor<R1, R2> operator|(R1, R2) {
    return {};
}

template<RuleType R1, RuleType... R2s>
[[nodiscard]] constexpr Sor<R1, R2s...> operator|(R1, Sor<R2s...>) {
    return {};
}

template<RuleType... R1s, RuleType R2>
[[nodiscard]] constexpr Sor<R1s..., R2> operator|(Sor<R1s...>, R2) {
    return {};
}

template<RuleType... R1s, RuleType... R2s>
[[nodiscard]] constexpr Sor<R1s..., R2s...> operator|(Sor<R1s...>, Sor<R2s...>) {
    return {};
}

/// Equivalent to `R & *(S & R)`.
template<RuleType R, RuleType S>
[[nodiscard]] constexpr auto list(R r, S s) {
    return r & *(s & r);
}

/// Equivalent to `R & *((P & S & P) & R)`.
template<RuleType R, RuleType S, RuleType P>
[[nodiscard]] constexpr auto list(R r, S s, P p) {
    return r & *((p & s & p) & r);
}

/// Equivalent to `R & S & Rs[0] & S & Rs[1] & ...`.
template<RuleType S, RuleType R, RuleType... Rs>
[[nodiscard]] constexpr auto join(S s, R r, Rs... rs) {
    return r & ((s & rs) & ...);
}

}  // namespace qcpc
