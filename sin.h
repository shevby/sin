#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <initializer_list>

#include "sin_value.h"
#include "switch.h"

#define SIN_DEFINE_STANDARD_TYPE_SETTER_GETTER(SIN_TYPE, STANDARD_TYPE) \
    Sin(const STANDARD_TYPE &value);                                    \
    void operator=(STANDARD_TYPE &value);                               \
    STANDARD_TYPE as##SIN_TYPE() const;

class Sin
{
    std::shared_ptr<SinValue> _value;
    std::string _type;

    std::string _toString(int pads = 0);

public:
    Sin();
    Sin(const char *str);
    Sin(std::initializer_list<Sin> list);

    static Sin parse(const std::string &str);

    std::string type();

    SIN_DEFINE_STANDARD_TYPE_SETTER_GETTER(Uint8, uint8_t);
    SIN_DEFINE_STANDARD_TYPE_SETTER_GETTER(Int8, int8_t);
    SIN_DEFINE_STANDARD_TYPE_SETTER_GETTER(Uint16, uint16_t);
    SIN_DEFINE_STANDARD_TYPE_SETTER_GETTER(Int16, int16_t);
    SIN_DEFINE_STANDARD_TYPE_SETTER_GETTER(Uint32, uint32_t);
    SIN_DEFINE_STANDARD_TYPE_SETTER_GETTER(Int32, int32_t);
    SIN_DEFINE_STANDARD_TYPE_SETTER_GETTER(Uint64, uint64_t);
    SIN_DEFINE_STANDARD_TYPE_SETTER_GETTER(Int64, int64_t);
    SIN_DEFINE_STANDARD_TYPE_SETTER_GETTER(Float, float);
    SIN_DEFINE_STANDARD_TYPE_SETTER_GETTER(Double, double);
    SIN_DEFINE_STANDARD_TYPE_SETTER_GETTER(String, std::string);
    SIN_DEFINE_STANDARD_TYPE_SETTER_GETTER(Bool, bool);

    std::vector<Sin> &asArray();

    std::map<std::string, Sin> &asObject();

    Sin &operator[](const int index);

    Sin &operator[](const std::string &key);

    std::string toString();

    static Sin Array();

    static Sin Object();

private:
    std::string numberToString(int pads = 0);
    std::string stringToString();
    std::string arrayToString(int pads = 0);
    std::string objectToString(int pads = 0);
};
