#pragma once

#include <unordered_map>
#include <cstdint>
#include <string>

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


SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Uint8Value, uint8_t);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Int8Value, int8_t);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Uint16Value, uint16_t);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Int16Value, int16_t);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Uint32Value, uint32_t);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Int32Value, int32_t);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Uint64Value, uint64_t);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(Int64Value, int64_t);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(FloatValue, float);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(DoubleValue, double);
SIN_DEFINE_STANDARD_TYPE_SIN_VALUE(StringValue, std::string);


struct ArrayValue : SinValue {

};

struct ObjectValue : SinValue {

};
