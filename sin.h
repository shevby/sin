#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <initializer_list>

#include "sin_value.h"
#include "switch.h"

#define SIN_DEFINE_STANDARD_TYPE_SETTER_GETTER(SIN_TYPE, STANDARD_TYPE)\
    Sin(const STANDARD_TYPE & value);\
    void operator=(STANDARD_TYPE & value);\
    STANDARD_TYPE as ## SIN_TYPE() const;

class Sin
{
    std::shared_ptr<SinValue> _value;
    std::string _type;

    std::string _toString(int pads = 0);

public:
    Sin();
    Sin(const char * str);
    Sin(std::initializer_list<Sin> list);

    std::string type() {
        return _type;
    }

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

    std::vector<Sin> & asArray() {
        if(_type != "Array") {
            std::cout << "Type assertion. Requested type: Array, Actual type: " + _type << std::endl << std::endl;
            throw "Type assertion. Requested type: Array, Actual type: " + _type;
        }
        return dynamic_cast<Array*>(_value.get())->value;
    }

    std::map<std::string, Sin> & asObject() {
        if(_type != "Object") {
            std::cout << "Type assertion. Requested type: Object, Actual type: " + _type << std::endl << std::endl;
            throw "Type assertion. Requested type: Object, Actual type: " + _type;
        }
        return dynamic_cast<Object*>(_value.get())->value;
    }

    Sin & operator[](const int index) {

        if(_type != "Array") {
            _type = "Array";
            _value = std::make_shared<Array>();

            auto v = dynamic_cast<Array*>(_value.get());

            for(int i = 0; i <= index; i++) {
                v->value.push_back(Sin{});
            }
        }
        else if(asArray().size() - 1 < index) {
            auto left = index - (asArray().size() - 1);
            for(int i = 0; i < left; i++) {
                asArray().push_back(Sin{});
            }
        }

        return dynamic_cast<Array*>(_value.get())->value[index];
    }

    Sin & operator[](const std::string & key) {

        if(_type != "Object") {
            _type = "Object";
            _value = std::make_shared<Object>();
        }

        auto v = dynamic_cast<Object*>(_value.get())->value;
        
        if(!v.contains(key)) {
            v[key] = Sin{};
        }

        return dynamic_cast<Object*>(_value.get())->value[key];
    }

    std::string toString();

private:
    std::string numberToString(int pads = 0);
    std::string stringToString();
    std::string arrayToString(int pads = 0);
    std::string objectToString(int pads = 0);
};
