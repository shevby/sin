#include "sin.h"

Sin::Sin() {
    _type = "Object";
    _value = std::make_shared<Object>();
}

Sin::Sin(const char * str) {
    _type = "String";
    _value = std::make_shared<String>(std::string(str));
}

Sin::Sin(std::initializer_list<Sin> list) {
    _type = "Array";
    _value = std::make_shared<Array>();

    for(auto el : list) {
        dynamic_cast<Array*>(_value.get())->value.push_back(el);
    }
}

#define SIN_STANDARD_TYPE_SETTER_GETTER(SIN_TYPE, STANDARD_TYPE)\
    Sin::Sin(const STANDARD_TYPE & value) {\
        _type = #SIN_TYPE;\
        _value = std::make_shared<SIN_TYPE>(value);\
    }\
    void Sin::operator=(STANDARD_TYPE & value){\
        _type = #SIN_TYPE;\
        _value = std::make_shared<SIN_TYPE>(value);\
    }\
    STANDARD_TYPE Sin::as ## SIN_TYPE() const {\
        if(_type != #SIN_TYPE) {\
            std::cout << "\nType assertion. Requested type: " #SIN_TYPE ", Actual type: " + _type << std::endl << std::endl;\
            throw "Type assertion. Requested type: " #SIN_TYPE ", Actual type: " + _type;\
        }\
        return dynamic_cast<SIN_TYPE*>(_value.get())->value;\
    }

SIN_STANDARD_TYPE_SETTER_GETTER(Uint8, uint8_t);
SIN_STANDARD_TYPE_SETTER_GETTER(Int8, int8_t);
SIN_STANDARD_TYPE_SETTER_GETTER(Uint16, uint16_t);
SIN_STANDARD_TYPE_SETTER_GETTER(Int16, int16_t);
SIN_STANDARD_TYPE_SETTER_GETTER(Uint32, uint32_t);
SIN_STANDARD_TYPE_SETTER_GETTER(Int32, int32_t);
SIN_STANDARD_TYPE_SETTER_GETTER(Uint64, uint64_t);
SIN_STANDARD_TYPE_SETTER_GETTER(Int64, int64_t);
SIN_STANDARD_TYPE_SETTER_GETTER(Float, float);
SIN_STANDARD_TYPE_SETTER_GETTER(Double, double);
SIN_STANDARD_TYPE_SETTER_GETTER(String, std::string);
SIN_STANDARD_TYPE_SETTER_GETTER(Bool, bool);

std::string Sin::_toString(int pads, std::string path) {
    if (_type == "Object") {
        return objectToString(*this, pads);
    }

    if (_type == "Array") {
        return arrayToString(*this, pads);
    }

    if (_type == "String") {
        return stringToString(*this);
    }

    return numberToString(*this, pads);
}

std::string Sin::toString() {
    return this->_toString();
}

const int PAD_SIZE = 2;
const char PAD_CHAR = ' ';

static std::string getPadStr(int pads) {
    std::string padStr = "";

    for(int i = 0; i < pads * PAD_SIZE; i++) {
        padStr += PAD_CHAR;
    }

    return padStr;
}

std::string Sin::numberToString(const Sin & s, int pads) {

#define NUMBER_CASE(SIN_TYPE)\
.Case(#SIN_TYPE, [&]()->void{\
strValue = std::to_string(s.as ##SIN_TYPE());\
})

    std::string strValue;

    Switch<std::string>(s._type)
    NUMBER_CASE(Uint8)
    NUMBER_CASE(Int8)
    NUMBER_CASE(Uint16)
    NUMBER_CASE(Int16)
    NUMBER_CASE(Uint32)
    NUMBER_CASE(Int32)
    NUMBER_CASE(Uint64)
    NUMBER_CASE(Int64)
    NUMBER_CASE(Float)
    NUMBER_CASE(Double)
    .Case("Bool", [&]()->void{
        strValue = s.asBool() ? "true" : "false";
    })
    .exec();

    auto padStr = getPadStr(pads);

    std::string result;

    if(s._type == "Double" || s._type == "Int32" || s._type == "Bool") {
        result = ": " + strValue + "\n";
    }
    else {
        result = ": " + s._type + "\n" + padStr + strValue + "\n";
    }

    return result;
}

static std::string escapeString(std::string input) {
    std::string result;
    result.reserve(static_cast<size_t>(input.size() * 1.6));

    for(int i = 0; i < input.size(); i++) {
        switch(input[i]) {
            case '\t':
            result += "\\t";
            break;
            case '\n':
            result += "\\n";
            break;
            case '\r':
            result += "\\r";
            break;
            case '"':
            result += "\\\"";
            break;
            case '\\':
            result += "\\\\";
            break;
            default:
            result += input[i];
        }
    }

    return result;
}

std::string Sin::stringToString(const Sin & s) {
    std::string input = s.asString();

    return ": \"" + escapeString(input) + "\"\n";
}

std::string Sin::arrayToString(Sin & s, int pads) {
    std::string result = ": [\n";
    auto v = s.asArray();

    auto padStr = getPadStr(pads + 1);

    for(int i = 0; i < v.size(); i++) {
        result += padStr + "[" + std::to_string(i) + "]" + v[i]._toString(pads + 1, "[" + std::to_string(i) + "]");
    }

    result += padStr.substr(0, pads * PAD_SIZE) + "]\n";

    return result;
}

std::string Sin::objectToString(Sin & s, int pads, std::string path) {
    std::string result = ": {\n";
    auto object = s.asObject();

    auto padStr = getPadStr(pads + 1);

    for(auto el : object) {
        auto newPath = path + (el.first.find(' ') == std::string::npos ? ("." + el.first) : ("[\"" + escapeString(el.first) + "\"]"));
        result += padStr + newPath + el.second._toString(pads + 1, newPath);
    }

    result += padStr.substr(0, pads * PAD_SIZE) + "}\n";
    return result;
}
