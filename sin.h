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
#include "switch.h"

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


const int PAD_SIZE = 2;
const char PAD_CHAR = ' ';

std::string getPadStr(int pads = 0) {
    std::string padStr = "";

    for(int i = 0; i < pads * PAD_SIZE; i++) {
        padStr += PAD_CHAR;
    }

    return padStr;
}

class Sin
{

    std::shared_ptr<SinValue> _value;
    std::string _type;

    std::string _toString(int pads = 0, std::string path = "") {
        if(_type != "Array" && _type != "Object") {

            if(_type == "String") {
                return stringToString(*this);
            }

            return numberToString(*this, pads);
        }

        if(_type == "Array") {
            return arrayToString(*this, pads);
        }

        return objectToString(*this, pads);
    }

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

    std::string toString() {
        return this->_toString();
    }

    static std::string numberToString(const Sin & s, int pads = 0) {

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
        .exec();

        auto padStr = getPadStr(pads);

        return ": " + s._type + "\n" + padStr + strValue + "\n";
    }

    static std::string stringToString(const Sin & s) {
        return ": `" + s.asString() + "`\n";
    }

    std::string arrayToString(Sin & s, int pads = 0) {
        std::string result = ": [\n";
        auto v = s.asArray();

        auto padStr = getPadStr(pads + 1);
        
        for(int i = 0; i < v.size(); i++) {
            result += padStr + "[" + std::to_string(i) + "]" + v[i]._toString(pads + 1, "[" + std::to_string(i) + "]");
        }

        result += padStr.substr(0, pads * PAD_SIZE) + "]\n";

        return result;
    }

    std::string objectToString(Sin & s, int pads = 0, std::string path = "") {
        std::string result = ": {\n";
        auto object = s.asObject();

        auto padStr = getPadStr(pads + 1);
        
        for(auto el : object) {
            auto newPath = path + (el.first.find(' ') == std::string::npos ? ("." + el.first) : ("[" + el.first + "]"));
            result += padStr + newPath + el.second._toString(pads + 1, newPath);
        }

        result += padStr.substr(0, pads * PAD_SIZE) + "}\n";
        return result;
    }

};


int testSin() {
    using namespace std;
    Sin a;

    a["first"]["third one"] = 5;
    a["first"]["second one"] = {1,2,3,4};
    a["first"]["third one"]["another"] = "hey";
    a["first"]["second one"].asArray().push_back("there");

    cout << a.toString() << endl;
    

    return 0;
}