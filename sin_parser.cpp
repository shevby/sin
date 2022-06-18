#include <memory>
#include <vector>

#include "sin.h"
#include "sin_parser.h"
#include "sin_value.h"

const std::set<char> whitespace = {' ', '\t', '\n', '\r'};
const std::set<char> whitespace_or_colon = {' ', '\t', '\n', '\r', ':'};

int SinParser::get_char()
{
    if (ss.eof())
    {
        return EOF;
    }
    int ch = ss.get();
    if (ch == '\n')
    {
        line_number++;
    }
    return ch;
}

void SinParser::skip_chars(const std::set<char> &chars)
{
    while (!ss.eof())
    {
        char ch = ss.peek();
        if (chars.contains(ch))
        {
            get_char();
        }
        else
        {
            break;
        }
    }
}

void SinParser::skip_whitespace()
{
    skip_chars(whitespace);
}

std::string SinParser::read_till_char(const std::set<char> &chars)
{
    std::string res;
    while (true)
    {
        int ch = ss.peek();
        if (chars.contains(ch) || (ch == EOF))
        {
            break;
        }
        else
        {
            res += ch;
            get_char();
        }
    }
    return res;
}

std::string SinParser::read_till_char_with_escape(const std::set<char> &terminating_chars, const std::map<char, std::string> &escapes)
{
    std::string res;
    while (true)
    {
        int ch = ss.peek();
        if (terminating_chars.contains(ch) || (ch == EOF))
        {
            break;
        }
        else
        {
            get_char();
            if (ch == '\\')
            {
                ch = get_char();
                if (ch == EOF)
                {
                    error += "\nUnexpected EOF after backslash at line " + std::to_string(line_number);
                    return res;
                }
                res += escapes.contains(ch) ? escapes.at(ch) : "\\" + std::string(1, ch);
            }
            else
            {
                res += ch;
            }
        }
    }
    return res;
}

static bool char_is_alpha(char ch)
{
    if ((ch >= 'a') && (ch <= 'z'))
    {
        return true;
    }
    if ((ch >= 'A') && (ch <= 'Z'))
    {
        return true;
    }
    return false;
}

static bool char_is_num(char ch)
{
    return (ch >= '0') && (ch <= '9');
}

std::string SinParser::read_var_name()
{
    skip_whitespace();
    if (ss.eof())
    {
        error += "\nReading variable name: EOF at line: " + std::to_string(line_number);
        return {};
    }
    int ch = ss.peek();
    if (ch == '[')
    {
        get_char(); // [
        skip_whitespace();
        std::string res;
        ch = ss.peek();
        if ((ch == '`') || (ch == '"'))
        {
            res = read_string();
            skip_whitespace();
        }
        else
        {
            res = read_till_char({']'});
        }
        ch = get_char();
        if (ch != ']')
        {
            error += "\n']' expected but " + (ch == EOF ? "EOF" : "'" + std::string(1, ch) + "'") + " found at line " + std::to_string(line_number);
        }
        return res;
    }
    else if (ch == '.')
    {
        get_char(); // .
        return read_till_char(whitespace_or_colon);
    }
    else
    {
        error += std::string("\nReading variable name: unexpected char '") + (char)ch + "' at line: " + std::to_string(line_number);
        return {};
    }
}

std::string SinParser::read_var_type()
{
    skip_whitespace();
    if (ss.eof())
    {
        error += "\nReading variable type: EOF at line: " + std::to_string(line_number);
        return {};
    }
    return read_till_char(whitespace);
}

std::string SinParser::read_number()
{
    skip_whitespace();
    auto delimiters = whitespace;
    delimiters.insert(':');
    delimiters.insert('}');
    return read_till_char(delimiters);
}

std::string SinParser::read_string()
{
    int ch = get_char();
    if (ch == '\"')
    {
        std::string value = read_till_char_with_escape({'"'}, {{'n', "\n"}, {'"', "\""}, {'r', "\r"}, {'t', "\t"}, {'\\', "\\"}});
        get_char(); // closing "
        return value;
    }
    else if (ch == '`')
    {
        std::string value = read_till_char_with_escape({'`'}, {{'`', "`"}, {'\\', "\\"}});
        if (value.size() && value[0] == '\n')
        {
            value.erase(value.begin());
        }
        if (value.size() && *value.rbegin() == '\n')
        {
            value.pop_back();
        }
        get_char(); // closing `
        return value;
    }
    else
    {
        error += "\ncharacter " + (ch == EOF ? "EOF" : "'" + std::string(1, ch) + "'") + " is not a valid quote at line " + std::to_string(line_number);
        return {};
    }
}

Sin SinParser::read_sin_value()
{
    skip_whitespace();
    if (ss.eof())
    {
        error += "\nEOF while reading SIN value at line " + std::to_string(line_number);
        return {};
    }

    int ch = get_char();
    if (ch == EOF)
    {
        error += "\n':' expected but EOF encountered at line " + std::to_string(line_number);
        return {};
    }
    if (ch != ':')
    {
        error += std::string("\n':' expected but ") + (char)ch + " encountered at line " + std::to_string(line_number);
        return {};
    }
    skip_whitespace();

    ch = ss.peek();
    if (ch == EOF)
    {
        error += "\nUnexpected EOF encountered at line " + std::to_string(line_number);
        return {};
    }
    else if ((ch == '\"') || (ch == '`'))
    {
        return read_string();
    }
    else if (char_is_alpha(ch) || char_is_num(ch) || (ch == '-') || (ch == '+'))
    {
        std::string sin_type = read_var_type();
        if (sin_type == "false")
        {
            return false;
        }
        if (sin_type == "true")
        {
            return true;
        }
        if (sin_type == "")
        {
            error += "\ntype is empty at line " + std::to_string(line_number);
            return {};
        }

        // if it looks like a number
        // try to parse as integer, then as double
        static std::set<char> first_char_of_number = {'-', '+', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
        if (first_char_of_number.contains(sin_type[0]))
        {
#define PARSE_NUMBER_TYPE_NOT_SPECIFIED(STANDARD_TYPE, CONVERTER)    \
    try                                                              \
    {                                                                \
        size_t end_of_var;                                           \
        STANDARD_TYPE value = std::CONVERTER(sin_type, &end_of_var); \
        if (end_of_var != sin_type.size())                           \
        {                                                            \
            throw std::runtime_error("not parsed");                  \
        }                                                            \
        return value;                                                \
    }                                                                \
    catch (...)                                                      \
    {                                                                \
    }

            PARSE_NUMBER_TYPE_NOT_SPECIFIED(int, stoi);
            PARSE_NUMBER_TYPE_NOT_SPECIFIED(int64_t, stoll);
            PARSE_NUMBER_TYPE_NOT_SPECIFIED(uint64_t, stoull);
            PARSE_NUMBER_TYPE_NOT_SPECIFIED(double, stod);

            error += "\nCannot parse number: '" + sin_type + "' at line " + std::to_string(line_number);
            return {};
        }

        skip_whitespace();
        std::string sin_value = read_number();

        if (sin_value == "")
        {
            error += "\nNumber is empty at line " + std::to_string(line_number);
            return {};
        }

        // Unsigned numbers starting with '-' are not valid
        if (sin_type[0] == 'U' && sin_value[0] == '-')
        {
            error += std::string("\nOut of bounds. Can't parse '") + sin_value + "' at line " + std::to_string(line_number);
            return {};
        }

        if (sin_type == "Bool")
        {
            if (sin_value == "false")
            {
                return false;
            }
            if (sin_value == "true")
            {
                return true;
            }
            error += "\nInvalid boolean value: " + sin_value + " at line " + std::to_string(line_number);
            return {};
        }
#define PARSE_NUMBER(SIN_TYPE, STANDARD_TYPE, CONVERTER, CHECK_LIMIT, MIN_VALUE, MAX_VALUE)                                      \
    else if (sin_type == #SIN_TYPE)                                                                                              \
    {                                                                                                                            \
        try                                                                                                                      \
        {                                                                                                                        \
            auto value = std::CONVERTER(sin_value);                                                                              \
            if (CHECK_LIMIT && (value < MIN_VALUE || value > MAX_VALUE))                                                         \
            {                                                                                                                    \
                error += std::string("\nOut of bounds. Can't parse '") + sin_value + "' at line " + std::to_string(line_number); \
                return {};                                                                                                       \
            }                                                                                                                    \
            return (STANDARD_TYPE)value;                                                                                         \
        }                                                                                                                        \
        catch (std::exception & e)                                                                                               \
        {                                                                                                                        \
            error += std::string("\n") + e.what() + ". Can't parse '" + sin_value + "' at line " + std::to_string(line_number);  \
            return {};                                                                                                           \
        }                                                                                                                        \
    }

        PARSE_NUMBER(Int8, int8_t, stoll, true, -0x80ll, 0x7Fll)
        PARSE_NUMBER(Int16, int16_t, stoll, true, -0x8000ll, 0x7FFFll)
        PARSE_NUMBER(Int32, int32_t, stoll, true, -0x80000000ll, 0x7FFFFFFFll)
        PARSE_NUMBER(Int64, int64_t, stoll, false, 0, 0)

        PARSE_NUMBER(Uint8, uint8_t, stoull, true, 0, 0xFFull)
        PARSE_NUMBER(Uint16, uint16_t, stoull, true, 0, 0xFFFFull)
        PARSE_NUMBER(Uint32, uint32_t, stoull, true, 0, 0xFFFFFFFFull)
        PARSE_NUMBER(Uint64, uint64_t, stoull, false, 0, 0)

        PARSE_NUMBER(Double, double, stod, false, 0, 0)

        else
        {
            error += "\nUnsupported type: " + sin_type + " at line " + std::to_string(line_number);
            return {};
        }
    }
    else if (ch == '{')
    {
        get_char();
        Sin sin;
        while (true)
        {
            skip_whitespace();
            int ch2 = ss.peek();
            if (ch2 == EOF)
            {
                error += "\nEOF at line " + std::to_string(line_number);
                return sin;
            }
            else if ((ch2 == '.') || (ch2 == '['))
            {
                auto name = read_var_name();
                skip_whitespace();
                Sin value = read_sin_value();
                sin[name] = value;
            }
            else if (ch2 == '}')
            {
                get_char(); // }
                return sin;
            }
            else
            {
                error += std::string("\nUnexpected characted '") + (char)ch2 + "' encountered at line " + std::to_string(line_number);
                return sin;
            }
        }
    }
    else if (ch == '[')
    {
        get_char();
        Sin sin = {Sin()};
        while (true)
        {
            skip_whitespace();
            int ch2 = ss.peek();
            if (ch2 == EOF)
            {
                error += "\nEOF at line " + std::to_string(line_number);
                return sin;
            }
            else if ((ch2 == '.') || (ch2 == '['))
            {
                auto name = read_var_name();
                int32_t index;
                try
                {
                    size_t end_of_int;
                    index = std::stoi(name, &end_of_int);
                    if (end_of_int != name.size())
                    {
                        throw std::invalid_argument("Extra characters");
                    }
                    if (index < 0)
                    {
                        throw new std::invalid_argument("Index < 0");
                    }
                }
                catch (std::invalid_argument &e)
                {
                    error += std::string("\n") + e.what() + ". Invalid array index: '" + name + "' at line " + std::to_string(line_number);
                    return {};
                }
                skip_whitespace();
                Sin value = read_sin_value();
                sin[index] = value;
            }
            else if (ch2 == ']')
            {
                get_char(); // ]
                return sin;
            }
            else
            {
                error += std::string("\nUnexpected characted '") + (char)ch2 + "' encountered at line " + std::to_string(line_number);
                return sin;
            }
        }
        return sin;
    }
    else
    {
        error += std::string("\nUnexpected characted '") + (char)ch + "' encountered at line " + std::to_string(line_number);
        return {};
    }
}

SinParser::SinParser(const std::string &str) : ss(str)
{
    value = read_sin_value();
}
