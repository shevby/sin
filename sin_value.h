#pragma once

#include <unordered_map>
#include <cstdint>
#include <string>
#include <vector>

class Sin;

#define SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(SIN_TYPE, STANDARD_TYPE) \
    struct SIN_TYPE : SinValue {\
        STANDARD_TYPE value;\
        SIN_TYPE(STANDARD_TYPE v): SinValue{}, value{v} {}\
    }



struct SinValue {
    SinValue(){};
    virtual ~SinValue(){};
};


SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Uint8, uint8_t);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Int8, int8_t);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Uint16, uint16_t);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Int16, int16_t);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Uint32, uint32_t);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Int32, int32_t);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Uint64, uint64_t);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Int64, int64_t);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Float, float);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Double, double);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(String, std::string);



struct Array : SinValue {
    std::vector<Sin> value;
};

struct Object : SinValue {
    std::unordered_map<std::string, Sin> value;
};

struct Undefined : SinValue {
    const std::string value = "Undefined";
};
