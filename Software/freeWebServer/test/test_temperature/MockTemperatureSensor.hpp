#ifndef MOCKTEMPERATURESENSOR_H
#define MOCKTEMPERATURESENSOR_H

#include "ITemperatureSensor.hpp"

class MockTemperatureSensor : public ITemperatureSensor {
private:
    float mockTemp;
    bool shouldRead;

public:
    MockTemperatureSensor(float temp, bool read) : mockTemp(temp), shouldRead(read) {}

    bool readTemp() override {
        return shouldRead;
    }

    float getTemp() override {
        return mockTemp;
    }
};

#endif // MOCKTEMPERATURESENSOR_H