#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <initializer_list>

#ifndef _GNU_SOURCE 
    #define _GNU_SOURCE 
#endif 

#include <boost/stacktrace.hpp>

#include "sin_value.h"

#define SIN_STANDARD_TYPE_SETTER(SIN_TYPE, STANDARD_TYPE)\
    Sin(const STANDARD_TYPE & value) {\
        _type = #SIN_TYPE;\
        _value = std::make_shared<SIN_TYPE>(value);\
    }\
    void operator=(STANDARD_TYPE & value){\
        _type = #SIN_TYPE;\
        _value = std::make_shared<SIN_TYPE>(value);\
    }

#define SIN_STANDARD_TYPE_GETTER(SIN_TYPE, STANDARD_TYPE)\
    STANDARD_TYPE as ## SIN_TYPE() const {\
        if(_type != #SIN_TYPE) {\
            std::cout << "\nType assertion. Requested type: " #SIN_TYPE ", Actual type: " + _type << std::endl << std::endl;\
            std::cout << "StackTrace:\n" << boost::stacktrace::stacktrace() << std::endl;\
            throw "Type assertion. Requested type: " #SIN_TYPE ", Actual type: " + _type;\ 
        }\
        return dynamic_cast<SIN_TYPE*>(_value.get())->value;\
    }


class Sin
{

    std::shared_ptr<SinValue> _value;
    std::string _type;

public:

    Sin() {
        _type = "Object";
        _value = std::make_shared<Object>();
    }

    std::string type() {
        return _type;
    }

    SIN_STANDARD_TYPE_SETTER(Uint8, uint8_t);
    SIN_STANDARD_TYPE_SETTER(Int8, int8_t);
    SIN_STANDARD_TYPE_SETTER(Uint16, uint16_t);
    SIN_STANDARD_TYPE_SETTER(Int16, int16_t);
    SIN_STANDARD_TYPE_SETTER(Uint32, uint32_t);
    SIN_STANDARD_TYPE_SETTER(Int32, int32_t);
    SIN_STANDARD_TYPE_SETTER(Uint64, uint64_t);
    SIN_STANDARD_TYPE_SETTER(Int64, int64_t);
    SIN_STANDARD_TYPE_SETTER(Float, float);
    SIN_STANDARD_TYPE_SETTER(Double, double);
    SIN_STANDARD_TYPE_SETTER(String, std::string);

    SIN_STANDARD_TYPE_GETTER(Uint8, uint8_t);
    SIN_STANDARD_TYPE_GETTER(Int8, int8_t);
    SIN_STANDARD_TYPE_GETTER(Uint16, uint16_t);
    SIN_STANDARD_TYPE_GETTER(Int16, int16_t);
    SIN_STANDARD_TYPE_GETTER(Uint32, uint32_t);
    SIN_STANDARD_TYPE_GETTER(Int32, int32_t);
    SIN_STANDARD_TYPE_GETTER(Uint64, uint64_t);
    SIN_STANDARD_TYPE_GETTER(Int64, int64_t);
    SIN_STANDARD_TYPE_GETTER(Float, float);
    SIN_STANDARD_TYPE_GETTER(Double, double);
    SIN_STANDARD_TYPE_GETTER(String, std::string);

    std::vector<Sin> & asArray() {
        if(_type != "Array") {
            std::cout << "Type assertion. Requested type: Array, Actual type: " + _type << std::endl << std::endl;
            std::cout << "StackTrace:\n" << boost::stacktrace::stacktrace() << std::endl;
            throw "Type assertion. Requested type: Array, Actual type: " + _type;
        }
        return dynamic_cast<Array*>(_value.get())->value;
    }

    std::unordered_map<std::string, Sin> & asObject() {
        if(_type != "Object") {
            std::cout << "Type assertion. Requested type: Object, Actual type: " + _type << std::endl << std::endl;
            std::cout << "StackTrace:\n" << boost::stacktrace::stacktrace() << std::endl;
            throw "Type assertion. Requested type: Object, Actual type: " + _type;
        }
        return dynamic_cast<Object*>(_value.get())->value;
    }

    Sin(const char * str) {
        _type = "String";
        _value = std::make_shared<String>(std::string(str));
    }

    Sin(std::initializer_list<Sin> list) {
        _type = "Array";
        _value = std::make_shared<Array>();

        for(auto el : list) {
            dynamic_cast<Array*>(_value.get())->value.push_back(el);
        }
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


};


int testSin() {
    using namespace std;
    Sin a = 5;
    a[10]["lol"] = 5;

    cout << a.asArray()[10].asObject()["lol"].asInt32() << endl;
    cout << a[10]["lol"].asInt32() << endl;

    return 0;
}