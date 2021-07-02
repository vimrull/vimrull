#include <vm/variable.h>
#include <catch2/catch2.hpp>

TEST_CASE("Test variable copy - type = value", "[variable]")
{
    variable var;
    var.type_ = value_type::INT64;
    var.value_.int64 = 0x12345678;

    auto var2 = var;
    REQUIRE(var.type_ == var2.type_);
    REQUIRE(var.value_.int64 == var2.value_.int64);
}

TEST_CASE("Test variable copy - type = STRING", "[variable]")
{
    variable var;
    var.type_ = value_type::STRING;
    var.str = "test";

    auto var2 = var;
    REQUIRE(var.type_ == var2.type_);
    REQUIRE(var.str.compare(var2.str) == 0);
}

TEST_CASE("Test variable copy - type = DATA", "[variable]")
{
    variable var;
    var.type_ = value_type::DATA;
    var.data = {'a', 'b', 'c'};

    auto var2 = var;
    REQUIRE(var.type_ == var2.type_);
    REQUIRE(var.data == var2.data);
}
