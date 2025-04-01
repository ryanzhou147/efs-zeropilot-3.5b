#pragma once

#include "stm32l5xx_hal.h"
#include "barometer_datatypes.hpp"
#include "barometer_iface.hpp"
#include "barometer_constants.cpp"

class Barometer : public IBarometer {
public:
	Barometer(I2C_HandleTypeDef* hi2c, GPIO_TypeDef *ps_port, uint16_t ps_pin);
	BarometerData_t readData() override;

private:
	//i2c handle
	I2C_HandleTypeDef* hi2c;

	//pressure, temperature data
    BarometerData_t readings;

    //check for communication failure
    bool setupDone;

    uint16_t prom_data[PROM_LENGTH];

    //private functions
    void reset();
    void getProm();
    uint16_t getPromElement(uint8_t address);
    bool getBarometer(uint8_t conversion_command, uint32_t &uncompensated_pressure_temperature);
};

