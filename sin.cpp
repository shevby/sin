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
