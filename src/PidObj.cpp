#include "PidObj.h"

PidObj::PidObj(int pidId, String name, String unit, int fDecimalPoint) {

    this->pidId = pidId;
    this->name = name;
    this->fDecimalPoint = fDecimalPoint;
}

PidObj::PidObj(int pidId, String name, String unit, int fDecimalPoint, float fMinValue, float fMaxValue) {

    //PidObj(pidId, name, unit, fDecimalPoint);
    this->fMinValue = fMinValue;
    this->fMaxValue = fMaxValue;
}

int PidObj::getPidId() {
    return pidId;
}

String PidObj::getName() {
    return name;
}

String PidObj::getUnit() {
    return unit;
}

float PidObj::getFValue() {
    return fValue;
}

void PidObj::setFValue(float value, unsigned long lastReadMs) {
    fValue = value;
    this->lastReadMs = lastReadMs;
}

int PidObj::getFDecimalPoint() {
    return fDecimalPoint;
}

float PidObj::getFMinValue() {
    return fMinValue;
}

float PidObj::getFMaxValue() {
    return fMaxValue;
}

unsigned long PidObj::getLastReadMs() {
    return lastReadMs;
}