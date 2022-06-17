#include "sin.h"

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
