#include "PidObj.h"
#include "config.h"

PidObj::PidObj(int pidId, const char* name, const char* unit, int fDecimalPoint) :
    pidId(pidId),
    name(name),
    unit(unit),
    fDecimalPoint(fDecimalPoint)
{}

PidObj::PidObj(int pidId, const char* name, const char* unit, int fDecimalPoint, float fMinValue, float fMaxValue, long minReadDelayFromEmlMs) :
    pidId(pidId),
    name(name),
    unit(unit),
    fDecimalPoint(fDecimalPoint),
    fMinValue(fMinValue),
    fMaxValue(fMaxValue),
    minReadDelayFromEmlMs(minReadDelayFromEmlMs)
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

void PidObj::setFValue(float value) {
    DBG_PRINT("Set: ");
    DBG_PRINTLN(value);
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

long PidObj::getMinReadDelayFromEmlMs() {
    return minReadDelayFromEmlMs;
}