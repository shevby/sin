#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <iostream>

#include "sin_value.h"
#include "sin.h"

class SinParser {
public:
    SinParser(const std::string &str);
    Sin value;
    std::string error;
private:
    size_t line_number = 0;
    std::stringstream ss;

    /**
     * A wrapper around std::stringsteam::get() to implement line counter
     */
    int get_char();
    std::set<char> whitespace =          {' ', '\t', '\n', '\r'};
    std::set<char> whitespace_or_colon = {' ', '\t', '\n', '\r', ':'};
    void skip_chars(const std::set<char> &chars);
    void skip_whitespace();
    std::string read_till_char(const std::set<char> &chars);
    std::string read_till_char_with_escape(const std::set<char> &terminating_chars, const std::map<char, std::string> &escapes);
    std::string read_number();

    Sin read_sin_value();
    std::string read_var_name();
    std::string read_var_type();
    std::string read_string();
};

int SinParser::get_char() {
    if (ss.eof()) {
        return EOF;
    }
    int ch = ss.get();
    if (ch == '\n') {
        line_number++;
    }
    return ch;
}

void SinParser::skip_chars(const std::set<char> &chars) {
    while (!ss.eof()) {
        char ch = ss.peek();
        if (chars.contains(ch)) {
            get_char();
        }
        else {
            break;
        }
    }
}

void SinParser::skip_whitespace() {
    skip_chars(whitespace);
}

std::string SinParser::read_till_char(const std::set<char> &chars) {
    std::string res;
    while (true) {
        int ch = ss.peek();
        if (chars.contains(ch) || (ch == EOF)) {
            break;
        }
        else {
            res += ch;
            get_char();
        }
    }
    return res;
}

std::string SinParser::read_till_char_with_escape(const std::set<char> &terminating_chars, const std::map<char, std::string> &escapes) {
    std::string res;
    while (true) {
        int ch = ss.peek();
        if (terminating_chars.contains(ch) || (ch == EOF)) {
            break;
        }
        else {
            get_char();
            if (ch == '\\') {
                ch = get_char();
                if (ch == EOF) {
                    error += "\nUnexpected EOF after backslash at line " + std::to_string(line_number);
                    return res;
                }
                res += escapes.contains(ch) ? escapes.at(ch) : "\\" + std::string(1, ch);
            }
            else {
                res += ch;
            }
        }
    }
    return res;
}

static bool char_is_alpha(char ch) {
    if ((ch >= 'a') && (ch <= 'z')) {
        return true;
    }
    if ((ch >= 'A') && (ch <= 'Z')) {
        return true;
    }
    return false;
}

std::string SinParser::read_var_name() {
    skip_whitespace();
    if (ss.eof()) {
        error += "\nReading variable name: EOF at line: " + std::to_string(line_number);
        return {};
    }
    int ch = ss.peek();
    if (ch == '[') {
        get_char(); // [
        skip_whitespace();
        std::string res;
        ch = ss.peek();
        if ((ch == '`') || (ch == '"')) {
            res = read_string();
            skip_whitespace();
        }
        else {
            res = read_till_char({']'});
        }
        ch = get_char();
        if (ch != ']') {
            error += "\n']' expected but " + (ch == EOF ? "EOF" : "'" + std::string(1, ch) + "'") + " found at line " + std::to_string(line_number);
        }
        return res;
    }
    else if (ch == '.') {
        get_char(); // .
        return read_till_char(whitespace_or_colon);
    }
    else {
        error += std::string("\nReading variable name: unexpected char '") + (char)ch + "' at line: " + std::to_string(line_number);
        return {};
    }
}

std::string SinParser::read_var_type() {
    skip_whitespace();
    if (ss.eof()) {
        error += "\nReading variable type: EOF at line: " + std::to_string(line_number);
        return {};
    }
    return read_till_char(whitespace);
}

std::string SinParser::read_number() {
    skip_whitespace();
    auto delimiters = whitespace;
    delimiters.insert(':');
    delimiters.insert('}');
    return read_till_char(delimiters);
}

std::string SinParser::read_string() {
    int ch = get_char();
    if (ch == '\"') {
        std::string value = read_till_char_with_escape({'"'}, {{'n', "\n"}, {'"', "\""}, {'r', "\r"}, {'t', "\t"}, {'\\', "\\"}});
        get_char(); // closing "
        return value;
    }
    else if (ch == '`') {
        std::string value = read_till_char_with_escape({'`'}, {{'`', "`"}, {'\\', "\\"}});
        if (value.size() && value[0] == '\n') {
            value.erase(value.begin());
        }
        if (value.size() && *value.rbegin() == '\n') {
            value.pop_back();
        }
        get_char(); // closing `
        return value;
    }
    else {
        error += "\ncharacter " + (ch == EOF ? "EOF" : "'" + std::string(1, ch) + "'") + " is not a valid quote at line " + std::to_string(line_number);
        return {};
    }
}

Sin SinParser::read_sin_value() {
    skip_whitespace();
    if (ss.eof()) {
        error += "\nEOF while reading SIN value at line " + std::to_string(line_number);
        return {};
    }

    int ch = get_char();
    if (ch == EOF) {
        error += "\n':' expected but EOF encountered at line " + std::to_string(line_number);
        return {};
    }
    if (ch != ':') {
        error += std::string("\n':' expected but ") + (char)ch + " encountered at line " + std::to_string(line_number);
        return {};
    }
    skip_whitespace();

    ch = ss.peek();
    if (ch == EOF) {
        error += "\nUnexpected EOF encountered at line " + std::to_string(line_number);
        return {};
    }
    else if ((ch == '\"') || (ch == '`')) {
        return read_string();
    }
    else if (char_is_alpha(ch) || (ch == '-') || (ch == '+')) {
        std::string sin_type = read_var_type();
        if (sin_type == "false") {
            return false;
        }
        if (sin_type == "true") {
            return true;
        }
        if (sin_type == "") {
            error += "\ntype is empty at line " + std::to_string(line_number);
            return {};
        }

        // if it looks like a number
        // try to parse as integer, then as double
        static std::set<char> first_char_of_number = {'-', '+', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
        if (first_char_of_number.contains(sin_type[0])) {
            try {
                size_t end_of_int;
                int64_t value = std::stoll(sin_type, &end_of_int);
                if (end_of_int != sin_type.size()) {
                    throw std::invalid_argument("Extra characters");
                }
                return value;
            }
            catch (std::invalid_argument &e1) {
                try {
                    return std::stod(sin_type);
                }
                catch (std::invalid_argument &e2) {
                    error +="\nCannot parse number: '" + sin_type + "' at line " + std::to_string(line_number);
                    return {};
                }
            }
        }

        skip_whitespace();
        std::string sin_value = read_number();

        if (sin_type == "Bool") {
            if (sin_value == "false") {
                return false;
            }
            if (sin_value == "true") {
                return true;
            }
            error += "\nInvalid boolean value: " + sin_value + " at line " + std::to_string(line_number);
            return {};
        }
#define PARSE_NUMBER(SIN_TYPE, STANDARD_TYPE, CONVERTER)                                                                            \
        else if (sin_type == #SIN_TYPE) {                                                                                           \
            try {                                                                                                                   \
                STANDARD_TYPE value = std::CONVERTER(sin_value);                                                                    \
                return value;                                                                                                       \
            }                                                                                                                       \
            catch (std::invalid_argument &e) {                                                                                      \
                error += std::string("\n") + e.what() + ". Can't parse '" + sin_value + "' at line " + std::to_string(line_number); \
                return {};                                                                                                          \
            }                                                                                                                       \
        }
        PARSE_NUMBER(Int8,  int8_t,  stoll)
        PARSE_NUMBER(Int16, int16_t, stoll)
        PARSE_NUMBER(Int32, int32_t, stoll)
        PARSE_NUMBER(Int64, int64_t, stoll)

        PARSE_NUMBER(Uint8,  int8_t,  stoull)
        PARSE_NUMBER(Uint16, int16_t, stoull)
        PARSE_NUMBER(Uint32, int32_t, stoull)
        PARSE_NUMBER(Uint64, int64_t, stoull)

        PARSE_NUMBER(Double, double, stod)

        else {
            error += "\nUnsupported type: " + sin_type + " at line " + std::to_string(line_number);
            return {};
        }
    }
    else if (ch == '{') {
        get_char();
        Sin sin;
        while (true) {
            skip_whitespace();
            int ch2 = ss.peek();
            if (ch2 == EOF) {
                error += "\nEOF at line " + std::to_string(line_number);
                return sin;
            }
            else if ((ch2 == '.') || (ch2 == '[')) {
                auto name = read_var_name();
                skip_whitespace();
                Sin value = read_sin_value();
                sin[name] = value;
            }
            else if (ch2 == '}') {
                get_char(); // }
                return sin;
            }
            else {
                error += std::string("\nUnexpected characted '") + (char)ch2 + "' encountered at line " + std::to_string(line_number);
                return sin;
            }
        }
    }
    else if (ch == '[') {
        get_char();
        Sin sin = {Sin()};
        while (true) {
            skip_whitespace();
            int ch2 = ss.peek();
            if (ch2 == EOF) {
                error += "\nEOF at line " + std::to_string(line_number);
                return sin;
            }
            else if ((ch2 == '.') || (ch2 == '[')) {
                auto name = read_var_name();
                int index;
                try {
                    index = std::stoi(name);
                    if (index < 0) {
                        throw new std::invalid_argument("Index < 0");
                    }
                }
                catch (std::invalid_argument &e) {
                    error += std::string("\n") + e.what() + ". Invalid array index: '" + name + "' at line " + std::to_string(line_number);
                    return {};
                }
                skip_whitespace();
                Sin value = read_sin_value();
                sin[index] = value;
            }
            else if (ch2 == ']') {
                get_char(); // ]
                return sin;
            }
            else {
                error += std::string("\nUnexpected characted '") + (char)ch2 + "' encountered at line " + std::to_string(line_number);
                return sin;
            }
        }
        return sin;
    }
    else {
        error += std::string("\nUnexpected characted '") + (char)ch + "' encountered at line " + std::to_string(line_number);
        return {};
    }
}

SinParser::SinParser(const std::string &str): ss(str) {
    value = read_sin_value();
}

static void str_assert(const std::string &real, const std::string &expected, const std::string &descr = "error") {
    if (expected != real) {
        std::cerr << descr << ":\n'" << real << "'\n != \n'" << expected << "'\n";
    }
}

// unit tests
int main() {
    SinParser sp1(":Int32\n1");
    str_assert(sp1.error, "", "Test 1");
    str_assert(sp1.value.type(), "Int32", "Test 1");
    str_assert(std::to_string(sp1.value.asInt32()), "1", "Test1");

    SinParser sp2(": {\n}\n");
    str_assert(sp2.error, "", "Test 2");
    str_assert(sp2.value.type(), "Object", "Test 2");

    SinParser sp3(": {\n"
                  "\t.element:{}}");
    str_assert(sp3.error, "", "Test 3");
    str_assert(sp3.value.type(), "Object", "Test 3");

    SinParser sp4(": {\n"
                  "\t.element:Int8 -33}");
    str_assert(sp4.error, "", "Test 4");
    str_assert(sp4.value.type(), "Object", "Test 4");
    str_assert(sp4.value["element"].type(), "Int8", "Test 4");
    str_assert(std::to_string(sp4.value["element"].asInt8()), "-33", "Test 4");

    std::string str5 = ": {\n"
                       "  .first: {\n"
                       "    [second one]: {\n"
                       "      [third one]: Int16\n"
                       "      12345\n"
                       "      [fourth one]: Int8\n"
                       "      0\n"
                       "    }\n"
                       "  }\n"
                       "}\n\n\n";
    SinParser sp5(str5);
    str_assert(sp5.error, "", "Test 5");
    str_assert(sp5.value.type(), "Object", "Test 5");
    str_assert(sp4.value["first"].type(), "Object", "Test 5");
    str_assert(std::to_string(sp5.value["first"]["second one"]["third one"].asInt16()), "12345", "Test 5");
    str_assert(std::to_string(sp5.value["first"]["second one"]["fourth one"].asInt8()), "0", "Test 5");

    std::string str6 = " :[\n"
                       "  [0]: Int32\n"
                       "  1\n"
                       "  [1]: Int32\n"
                       "  2\n"
                       "  [2]: Int32\n"
                       "  3\n"
                       "  [3]: Int32\n"
                       "  4\n"
                       "]";
    SinParser sp6(str6);
    str_assert(sp6.error, "", "Test 6");
    str_assert(sp6.value.type(), "Array", "Test 6");
    str_assert(std::to_string(sp6.value[2].asInt32()), "3", "Test 6");

    SinParser sp7(":[]");
    str_assert(sp7.error, "", "Test 7");
    str_assert(sp7.value.type(), "Array", "Test 7");

    SinParser sp8(" :\n"
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
    str_assert(sp8.error, "", "Test 8");
    str_assert(std::to_string(sp8.value[1]["one"]["a"].asInt8()), "3", "Test 8");
    str_assert(std::to_string(sp8.value[1]["one"]["b"][4].asInt32()), "7", "Test 8");
    str_assert(std::to_string(sp8.value[3].asInt8()), "4", "Test 8");

    SinParser sp9(":false");
    str_assert(sp9.error, "", "Test 9");
    str_assert(std::to_string(sp9.value.asBool()), "0", "Test 9");

    SinParser sp10(":Bool true");
    str_assert(sp10.error, "", "Test 10");
    str_assert(std::to_string(sp10.value.asBool()), "1", "Test 10");

    SinParser sp11(":{\n"
                   ".bool1: Bool false\n"
                   ".number: Double 1.23e+5\n"
                   ".bool2: true\n"
                   "}");
    str_assert(sp11.error, "", "Test 11");
    str_assert(std::to_string(sp11.value["bool1"].asBool()), "0", "Test 11");
    str_assert(std::to_string(sp11.value["bool2"].asBool()), "1", "Test 11");
    str_assert(std::to_string((int)sp11.value["number"].asDouble()), "123000", "Test 11");

    SinParser sp12(":-7467839     ");
    str_assert(sp12.error, "", "Test 12");
    str_assert(sp12.value.type(), "Int64", "Test 12");
    str_assert(std::to_string(sp12.value.asInt64()), "-7467839", "Test 12");

    SinParser sp13(":-7467839e+2     ");
    str_assert(sp13.error, "", "Test 13");
    str_assert(sp13.value.type(), "Double", "Test 13");
    str_assert(std::to_string((int64_t)sp13.value.asDouble()), "-746783900", "Test 13");

    SinParser sp14(": \"simple string\"     ");
    str_assert(sp14.error, "", "Test 14");
    str_assert(sp14.value.type(), "String", "Test 14");
    str_assert(sp14.value.asString(), "simple string", "Test 14");

    SinParser sp15(": `simple string`     ");
    str_assert(sp15.error, "", "Test 15");
    str_assert(sp15.value.type(), "String", "Test 15");
    str_assert(sp15.value.asString(), "simple string", "Test 15");

    SinParser sp16(": \"\\nstring\twith\\\\escaped\\\"symbols\"");
    str_assert(sp16.error, "", "Test 16");
    str_assert(sp16.value.type(), "String", "Test 16");
    str_assert(sp16.value.asString(), "\nstring\twith\\escaped\"symbols", "Test 16");

    SinParser sp17(": `\\nstring\\twith\\\\escaped\\`symbols`");
    str_assert(sp17.error, "", "Test 17");
    str_assert(sp17.value.type(), "String", "Test 17");
    str_assert(sp17.value.asString(), "\\nstring\\twith\\escaped`symbols", "Test 17");

    SinParser sp18(": [\n"
                   "    [20]: \"123\"\n"
                   "    [3] : \"456\"\n"
                   "    [0] : \"789\"\n"
                   "]");
    str_assert(sp18.error, "", "Test 18");
    str_assert(sp18.value[20].asString(), "123", "Test 18");
    str_assert(sp18.value[3].asString(), "456", "Test 18");
    str_assert(sp18.value[0].asString(), "789", "Test 18");

    SinParser sp19(": {\n"
                   " [ \" var 1 \" ] : -3\n"
                   "[\"var 2\"]: true   \n"
                   "   [ \"var \\\" 3\" ] : \"abc\"\n"
                   "}\n");
    str_assert(sp19.error, "", "Test 19");
    str_assert(std::to_string(sp19.value[" var 1 "].asInt64()), "-3", "Test 19");
    str_assert(std::to_string(sp19.value["var 2"].asBool()), "1", "Test 19");
    str_assert(sp19.value["var \" 3"].asString(), "abc", "Test 19");

    SinParser sp20(": `\n"
                   "abc\n"
                   "c\\\\d\n"
                   "ef\\`gh\n"
                   "`");
    str_assert(sp20.error, "", "Test 20");
    str_assert(sp20.value.asString(), "abc\nc\\d\nef`gh", "Test 20");

    SinParser sp21(": `ab\\`c`\n");
    str_assert(sp21.error, "", "Test 21");
    str_assert(sp21.value.asString(), "ab`c", "Test 21");
}
