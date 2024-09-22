#include "PidObj.h"

PidObj::PidObj(int pidId, const char* name, const char* unit, int fDecimalPoint) :
    pidId(pidId),
    name(name),
    unit(unit),
    fDecimalPoint(fDecimalPoint)
{}

PidObj::PidObj(int pidId, const char* name, const char* unit, int fDecimalPoint, float fMinValue, float fMaxValue) :
    pidId(pidId),
    name(name),
    unit(unit),
    fDecimalPoint(fDecimalPoint),
    fMinValue(fMinValue),
    fMaxValue(fMaxValue)
{}

int PidObj::getPidId() {
    return pidId;
}

String PidObj::getName() {
    return String(name);
}

String PidObj::getUnit() {
    return String(unit);
}

float PidObj::getFValue() {
    return fValue;
}

void PidObj::setFValue(float& value) {
    Serial.print("Set: ");
    Serial.println(value);
    fValue = value;
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