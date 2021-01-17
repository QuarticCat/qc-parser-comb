#include "gtest/gtest.h"
#include "qcpc/qcpc.hpp"

TEST(CttiTest, FnvHash) {
    using qcpc::detail::fnv_hash;
    ASSERT_EQ(fnv_hash("QC"), fnv_hash("QC"));
    ASSERT_NE(fnv_hash("QC"), fnv_hash("CQ"));
}

TEST(CttiTest, PrettyFunction) {
    using qcpc::detail::pretty_function;
    auto _a = pretty_function<int>();
    auto _b = pretty_function<float>();
}

TEST(CttiTest, TypeHash) {
    using qcpc::type_hash;
    ASSERT_EQ(type_hash<char>(), type_hash<char>());
    ASSERT_EQ(type_hash<int>(), type_hash<int>());
    ASSERT_EQ(type_hash<float>(), type_hash<float>());
    ASSERT_NE(type_hash<char>(), type_hash<int>());
    ASSERT_NE(type_hash<int>(), type_hash<float>());
    ASSERT_NE(type_hash<float>(), type_hash<char>());
}
