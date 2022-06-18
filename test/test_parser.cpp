#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

#include "sin_parser.h"

TEST_CASE("SIN parser: integer values out of bounds") {
    // Unsigned
    SECTION("Uint8 - Low") {
        SinParser sp(":Uint8 -1");
        CHECK(sp.error != "");
    }

    SECTION("Uint8 - High") {
        SinParser sp(":Uint8 256");
        CHECK(sp.error != "");
    }

    SECTION("Uint16 - Low") {
        SinParser sp(":Uint16 -1");
        CHECK(sp.error != "");
    }

    SECTION("Uint16 - High") {
        SinParser sp(":Uint16 65536");
        CHECK(sp.error != "");
    }

    SECTION("Uint32 - Low") {
        SinParser sp(":Uint32 -1");
        CHECK(sp.error != "");
    }

    SECTION("Uint32 - High") {
        SinParser sp(":Uint32 4294967296");
        CHECK(sp.error != "");
    }

    SECTION("Uint64 - Low") {
        SinParser sp(":Uint64 -1");
        CHECK(sp.error != "");
    }

    SECTION("Uint64 - High") {
        SinParser sp(":Uint64 18446744073709551616");
        CHECK(sp.error != "");
    }

    // Signed
    SECTION("Int8 - Low") {
        SinParser sp(":Int8 -129");
        CHECK(sp.error != "");
    }

    SECTION("Int8 - High") {
        SinParser sp(":Int8 128");
        CHECK(sp.error != "");
    }

    SECTION("Int16 - Low") {
        SinParser sp(":Int16 -32769");
        CHECK(sp.error != "");
    }

    SECTION("Int16 - High") {
        SinParser sp(":Int16 32768");
        CHECK(sp.error != "");
    }

    SECTION("Int32 - Low") {
        SinParser sp(":Int32 -2147483649");
        CHECK(sp.error != "");
    }

    SECTION("Int32 - High") {
        SinParser sp(":Int32 2147483648");
        CHECK(sp.error != "");
    }

    SECTION("Int - Low") {
        SinParser sp(":Int -2147483649");
        CHECK(sp.error != "");
    }

    SECTION("Int - High") {
        SinParser sp(":Int 2147483648");
        CHECK(sp.error != "");
    }

    SECTION("Int64 - Low") {
        SinParser sp(":Int64 -9223372036854775809");
        CHECK(sp.error != "");
    }

    SECTION("Int64 - High") {
        SinParser sp(":Int64 9223372036854775808");
        CHECK(sp.error != "");
    }
}

TEST_CASE("SIN parser") {
    SECTION("1") {
        SinParser sp(":Int32\n1");
        CHECK(sp.error == "");
        CHECK(sp.value.type() == "Int32");
        CHECK(sp.value.asInt32() == 1);
    }

    SECTION("2") {
        SinParser sp(": {\n}\n");
        CHECK(sp.error == "");
        CHECK(sp.value.type() == "Object");
    }

    SECTION("3") {
        SinParser sp(": {\n"
                     "\t.element:{}}");
        CHECK(sp.error == "");
        CHECK(sp.value.type() == "Object");
    }

    SECTION("4") {
        SinParser sp(": {\n"
                     "\t.element:Int8 -33}");
        CHECK(sp.error == "");
        CHECK(sp.value.type() == "Object");
        CHECK(sp.value["element"].type() == "Int8");
        CHECK(sp.value["element"].asInt8() == -33);
    }

    SECTION("5") {
        std::string str = ": {\n"
                          "  .first: {\n"
                          "    [second one]: {\n"
                          "      [third one]: Int16\n"
                          "      12345\n"
                          "      [fourth one]: Int8\n"
                          "      0\n"
                          "    }\n"
                          "  }\n"
                          "}\n\n\n";
        SinParser sp(str);
        CHECK(sp.error == "");
        CHECK(sp.value.type() == "Object");
        CHECK(sp.value["first"].type() == "Object");
        CHECK(sp.value["first"]["second one"]["third one"].asInt16() == 12345);
        CHECK(sp.value["first"]["second one"]["fourth one"].asInt8() == 0);
    }

    SECTION("6") {
        std::string str = " :[\n"
                          "  [0]: Int32\n"
                          "  1\n"
                          "  [1]: Int32\n"
                          "  2\n"
                          "  [2]: Int32\n"
                          "  3\n"
                          "  [3]: Int32\n"
                          "  4\n"
                          "]";
        SinParser sp(str);
        CHECK(sp.error == "");
        CHECK(sp.value.type() == "Array");
        CHECK(sp.value[2].asInt32() == 3);
    }

    SECTION("7") {
        SinParser sp(":[]");
        CHECK(sp.error == "");
        CHECK(sp.value.type() == "Array");
    }

    SECTION("8") {
        SinParser sp(" :\n"
                     "[\n"
                     "  [1] : {\n"
                     "      [one]: {\n"
                     "         .a : Int8 3 \n"
                     "         .b : [\n"
                     "           [1] : Int16 2\n"
                     "           [4] : Int32 7\n"
                     "         ]\n"
                     "      }\n"
                     "  }\n"
                     "  [3]: Int8 4\n"
                     "]");
        CHECK(sp.error == "");
        CHECK(sp.value[1]["one"]["a"].asInt8() == 3);
        CHECK(sp.value[1]["one"]["b"][4].asInt32() == 7);
        CHECK(sp.value[3].asInt8() == 4);
    }

    SECTION("9") {
        SinParser sp(":false");
        CHECK(sp.error == "");
        CHECK(sp.value.asBool() == 0);
    }

    SECTION("10") {
        SinParser sp(":Bool true");
        CHECK(sp.error == "");
        CHECK(sp.value.asBool() == 1);
    }

    SECTION("11") {
        SinParser sp(":{\n"
                     ".bool1: Bool false\n"
                     ".number: Double 1.23e+5\n"
                     ".bool2: true\n"
                     "}");
        CHECK(sp.error == "");
        CHECK(sp.value["bool1"].asBool() == 0);
        CHECK(sp.value["bool2"].asBool() == 1);
        CHECK(sp.value["number"].asDouble() == Catch::Approx(123000).margin(0.000001));
    }

    SECTION("12") {
        SinParser sp(":-7467839     ");
        CHECK(sp.error == "");
        CHECK(sp.value.type() == "Int32");
        CHECK(sp.value.asInt32() == -7467839);
    }

    SECTION("13") {
        SinParser sp(":-7467839e+2     ");
        CHECK(sp.error == "");
        CHECK(sp.value.type() == "Double");
        CHECK(sp.value.asDouble() == Catch::Approx(-746783900).margin(0.0000001));
    }

    SECTION("14") {
        SinParser sp14(": \"simple string\"     ");
        CHECK(sp14.error == "");
        CHECK(sp14.value.type() == "String");
        CHECK(sp14.value.asString() == "simple string");
    }

    SECTION("15") {
        SinParser sp(": `simple string`     ");
        CHECK(sp.error == "");
        CHECK(sp.value.type() == "String");
        CHECK(sp.value.asString() == "simple string");
    }

    SECTION("16") {
        SinParser sp(": \"\\nstring\twith\\\\escaped\\\"symbols\"");
        CHECK(sp.error == "");
        CHECK(sp.value.type() == "String");
        CHECK(sp.value.asString() == "\nstring\twith\\escaped\"symbols");
    }

    SECTION("17") {
        SinParser sp(": `\\nstring\\twith\\\\escaped\\`symbols`");
        CHECK(sp.error == "");
        CHECK(sp.value.type() == "String");
        CHECK(sp.value.asString() == "\\nstring\\twith\\escaped`symbols");
    }

    SECTION("18") {
        SinParser sp(": [\n"
                     "    [20]: \"123\"\n"
                     "    [3] : \"456\"\n"
                     "    [0] : \"789\"\n"
                     "]");
        CHECK(sp.error == "");
        CHECK(sp.value[20].asString() == "123");
        CHECK(sp.value[3].asString() == "456");
        CHECK(sp.value[0].asString() == "789");
    }

    SECTION("19") {
        SinParser sp(": {\n"
                     " [ \" var 1 \" ] : -3\n"
                     "[\"var 2\"]: true   \n"
                     "   [ \"var \\\" 3\" ] : \"abc\"\n"
                     "}\n");
        CHECK(sp.error == "");
        CHECK(sp.value[" var 1 "].asInt32() == -3);
        CHECK(sp.value["var 2"].asBool() == 1);
        CHECK(sp.value["var \" 3"].asString() == "abc");
    }

    SECTION("20") {
        SinParser sp(": `\n"
                     "abc\n"
                     "c\\\\d\n"
                     "ef\\`gh\n"
                     "`");
        CHECK(sp.error == "");
        CHECK(sp.value.asString() == "abc\nc\\d\nef`gh");
    }

    SECTION("21") {
        SinParser sp(": `ab\\`c`\n");
        CHECK(sp.error == "");
        CHECK(sp.value.asString() == "ab`c");
    }

    SECTION("22") {
        SinParser sp(": [\n"
                     "   [0extra_input] : true\n"
                     "  ]\n");
        CHECK(sp.error != "");
    }

    SECTION("23") {
        SinParser sp(": [\n"
                     "   [1] : 123\n"
                     "  ]\n");
        CHECK(sp.error == "");
        CHECK(sp.value[1].type() == "Int32");
        CHECK(sp.value[1].asInt32() == 123);
    }

    SECTION("24") {
        SinParser sp(": [\n"
                     "   [1] : 123extra_input\n"
                     "  ]\n");
        CHECK(sp.error != "");
    }

    SECTION("25") {
        SinParser sp(": [\n"
                     "   [1] : 123.456\n"
                     "  ]\n");
        CHECK(sp.error == "");
        CHECK(sp.value[1].type() == "Double");
        CHECK(sp.value[1].asDouble() == Catch::Approx(123.456).margin(0.000000001));
    }

    SECTION("26") {
        SinParser sp(": [\n"
                     "   [1] : 123.456extra\n"
                     "  ]\n");
        CHECK(sp.error != "");
    }
}
