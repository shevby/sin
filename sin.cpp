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

std::string getPadStr(int pads) {
    std::string padStr = "";

    for(int i = 0; i < pads * PAD_SIZE; i++) {
        padStr += PAD_CHAR;
    }

    return padStr;
}
