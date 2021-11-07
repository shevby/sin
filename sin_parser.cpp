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
    std::string read_number();

    Sin read_sin_value();
    std::string read_var_name();
    std::string read_var_type();
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
    while (!ss.eof()) {
        char ch = ss.peek();
        if (chars.contains(ch)) {
            break;
        }
        else {
            res += ch;
            get_char();
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
        auto res = read_till_char({']'});
        get_char(); // ]
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
    else if (ch == '\"') {
        // TODO : read string
        error += "\nStrings not implemented yet";
        return {};
    }
    else if (char_is_alpha(ch)) {
        std::string sin_type = read_var_type();
        skip_whitespace();
        std::string sin_value = read_number();

        if (false) {
            // dummy clause to use following macro
        }
#define PARSE_SMALL_INT(SIN_TYPE, STANDARD_TYPE)            \
        else if (sin_type == #SIN_TYPE) {                   \
            try {                                           \
                STANDARD_TYPE value = std::stoi(sin_value); \
                return value;                               \
            }                                               \
            catch (std::invalid_argument &e) {              \
                error += std::string("\n") + e.what() + ". Can't parse '" + sin_value + "' at line " + std::to_string(line_number); \
                return {};                                  \
            }                                               \
        }
        PARSE_SMALL_INT(Int8,  int8_t)
        PARSE_SMALL_INT(Int16, int16_t)
        PARSE_SMALL_INT(Int32, int32_t)
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
                    error += std::string("\n") + e.what() + ". Invalid array index: '" + name + "' at line " + std::to_string(line_number); \
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
}
