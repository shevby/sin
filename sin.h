#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "SinValue.h"

#define SIN_STANDARD_TYPE_SETTER(SIN_TYPE, STANDARD_TYPE)\
    void operator=(STANDARD_TYPE value){\
        _value = std::make_shared<SIN_TYPE>(value);\
    }

#define SIN_STANDARD_TYPE_GETTER(SIN_TYPE, STANDARD_TYPE)\
    STANDARD_TYPE get ## SIN_TYPE(){\
        return dynamic_cast<SIN_TYPE*>(_value.get())->value;\
    }

class Sin
{
public:
    enum class Types {
        UNDEFINED,
        UINT8,
        INT8,
        UINT16,
        INT16,
        UINT32,
        INT32,
        UINT64,
        INT64,
        FLOAT,
        DOUBLE,
        STRING,
        OBJECT,
        ARRAY
    };

    Sin::Types type;

    std::shared_ptr<SinValue> _value;

public:


    Sin();

    static Sin parse(std::string & str) {

    }

    static std::string build(Sin & sin) {

    }

    SIN_STANDARD_TYPE_SETTER(Uint8Value, uint8_t);
    SIN_STANDARD_TYPE_SETTER(Int8Value, int8_t);
    SIN_STANDARD_TYPE_SETTER(Uint16Value, uint16_t);
    SIN_STANDARD_TYPE_SETTER(Int16Value, int16_t);
    SIN_STANDARD_TYPE_SETTER(Uint32Value, uint32_t);
    SIN_STANDARD_TYPE_SETTER(Int32Value, int32_t);
    SIN_STANDARD_TYPE_SETTER(Uint64Value, uint64_t);
    SIN_STANDARD_TYPE_SETTER(Int64Value, int64_t);
    SIN_STANDARD_TYPE_SETTER(FloatValue, float);
    SIN_STANDARD_TYPE_SETTER(DoubleValue, double);
    SIN_STANDARD_TYPE_SETTER(StringValue, std::string);

    SIN_STANDARD_TYPE_GETTER(Uint8Value, uint8_t);
    SIN_STANDARD_TYPE_GETTER(Int8Value, int8_t);
    SIN_STANDARD_TYPE_GETTER(Uint16Value, uint16_t);
    SIN_STANDARD_TYPE_GETTER(Int16Value, int16_t);
    SIN_STANDARD_TYPE_GETTER(Uint32Value, uint32_t);
    SIN_STANDARD_TYPE_GETTER(Int32Value, int32_t);
    SIN_STANDARD_TYPE_GETTER(Uint64Value, uint64_t);
    SIN_STANDARD_TYPE_GETTER(Int64Value, int64_t);
    SIN_STANDARD_TYPE_GETTER(FloatValue, float);
    SIN_STANDARD_TYPE_GETTER(DoubleValue, double);
    SIN_STANDARD_TYPE_GETTER(StringValue, std::string);


};

int testSin() {

    return 0;
}