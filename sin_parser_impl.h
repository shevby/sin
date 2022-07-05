#include "sin.h"

#include <set>
#include <sstream>

class SinParser
{
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
