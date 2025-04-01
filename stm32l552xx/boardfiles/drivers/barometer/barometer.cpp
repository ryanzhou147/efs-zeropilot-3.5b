#include "barometer.hpp"
#include "barometer_datatypes.hpp"
#include "barometer_constants.cpp"

Barometer::Barometer(I2C_HandleTypeDef* hi2c, GPIO_TypeDef *ps_port, uint16_t ps_pin) : hi2c(hi2c){
	ps_port_ = ps_port;
	ps_pin_ = ps_pin;
	HAL_GPIO_WritePin(ps_port_, ps_pin_, GPIO_PIN_SET); //set ps pin high for I2C

	setupDone = true;
	getProm();
}

void Barometer::reset(){
	//i2c address, barometer address, reset command, #bytes, delay
	HAL_StatusTypeDef I2C_transfer = HAL_I2C_Master_Transmit(&hi2c1, MS5611_ADDRESS, &RESET_COMMAND, 1, HAL_MAX_DELAY);

	//track communication failure
	if (!I2C_transfer){
		setupDone = false;
	}

	//wait for reset to complete
	osDelay(3);
}

//read prom/get prom calibration coefficients
uint16_t Barometer::getPromElement(uint8_t address){
	uint8_t data[2];

	//i2c handle, address of sensor, address of prom coefficient, #bytes, delay
	HAL_StatusTypeDef I2C_transfer = HAL_I2C_Master_Transmit(&hi2c1, MS5611_ADDRESS, &address, 1, HAL_MAX_DELAY);
	if (!I2C_transfer){
		setupDone = false;
	}

	//i2c handle, address of sensor, where to receive prom, #bytes, delay
	HAL_StatusTypeDef I2C_transfer = HAL_I2C_Master_Receive(&hi2c1, MS5611_ADDRESS, data, 2, HAL_MAX_DELAY);
	if (!I2C_transfer){
		setupDone = false;
	}

	//1st byte is most significant
    return (data[0] << 8) | data[1];
}

void Barometer::getProm(){
	reset();
	for (int i=0; i<8; i++){
		prom_data[i] = getPromElement(PROM_READ_ADDRESS_0 + 2*i);
		if (!setupDone){
			return;
		}
	}
	setupDone = true;

}

//trigger conversion command and read adc
bool Barometer::getBarometer(uint8_t conversion_command, uint32_t &uncompensated_pressure_temperature){
    if (!setupDone) {
        return false;
    }

    //send conversion command
    HAL_StatusTypeDef I2C_transfer = HAL_I2C_Master_Transmit_DMA(&hi2c1, MS5611_ADDRESS, &conversion_command, 1);
    if (!I2C_transfer) {
        return false;
    }

    //wait for conversion to complete (at least 8.22ms for OSR=4096)
    osDelay(9);

    //send ADC read command
    HAL_StatusTypeDef I2C_transfer = HAL_I2C_Master_Transmit(&hi2c1, MS5611_ADDRESS, &ADC_READ, 1, HAL_MAX_DELAY);
    if (!I2C_transfer) {
        return false;
    }

    //read ADC value (3 bytes)
    uint8_t adc_data[3];
    HAL_StatusTypeDef I2C_transfer = HAL_I2C_Master_Receive_DMA(&hi2c1, MS5611_ADDRESS, adc_data, 3);
    if (!I2C_transfer) {
        return false;
    }

    osDelay(9);

	uncompensated_pressure_temperature = (adc_data[0] << 16) | (adc_data[1] << 8) | adc_data[2];


	return true;

}

//take uncompensated value from getBarometer and convert to actual temp/pressure value
BarometerData_t Barometer::readData(){
	BarometerData_t readings;
	uint32_t digital_pressure;
	uint32_t digital_temperature;


	//call command to get pressure/temperature, return large value for error
	if (!getBarometer(CONVERT_D2_OSR_4096, digital_temperature)){
		return FLT_MAX;
	}

	if (!getBarometer(CONVERT_D1_OSR_4096, digital_pressure)){
		return FLT_MAX;
	}

	//get calibration coefficients
	uint16_t pressure_sensitivity = prom_data[1];
	uint16_t pressure_offset = prom_data[2];
	uint16_t pres_sensitivity_temp_coeff = prom_data[3];
	uint16_t pres_offset_temp_coeff = prom_data[4];
	uint16_t reference_temperature = prom_data[5];
	uint16_t temp_coeff_of_temp = prom_data[6];

	//calculate pressure
	float dt = digital_temperature - (float)reference_temperature * TWO_POW_8;
	float f_pressure_offset = pressure_offset * TWO_POW_16 + (pres_offset_temp_coeff * dt) / TWO_POW_7;
	float f_pressure_sensitivity = pressure_sensitivity * TWO_POW_15  + (pres_sensitivity_temp_coeff * dt) / TWO_POW_8;
	float pres = ((digital_pressure * (f_pressure_sensitivity / TWO_POW_21) - f_pressure_offset) / TWO_POW_15)/100;
	readings.pressure = pres;

	//calculate temperature
	float temp = 2000 + dt * (float)temp_coeff_of_temp / TWO_POW_23;
	if (temp < 2000) {
		float t2 = (dt*dt) / TWO_POW_31;
		float pressure_offset_2 = 5.0f * pow((temp - 2000), 2) / 2;
		float pressure_sensitivity_2 = 5.0f * pow((temp - 2000), 2) / 4;

		if (temp < 1500) {
			pressure_offset_2 = pressure_offset_2 + 7 * pow((temp + 1500), 2);
			pressure_sensitivity_2 = pressure_sensitivity_2 + 11 * pow((temp + 1500), 2) / 2;
		}

		temp = temp - t2;
		f_pressure_offset = f_pressure_offset - pressure_offset_2;
		f_pressure_sensitivity = f_pressure_sensitivity - pressure_sensitivity_2;
	}
	temp /= 100.0f;
	readings.temperature = temp;

	return readings;



}
