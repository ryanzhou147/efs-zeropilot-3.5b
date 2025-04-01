#include <cstdint>

constexpr uint8_t MS5611_ADDRESS = 0x77 << 1;

//Reset sensor
constexpr uint8_t RESET_COMMAND = 0x1E;

// PROM Addresses -> calibration constants
constexpr uint8_t PROM_READ_ADDRESS_0 = 0xA0;
constexpr uint8_t PROM_READ_ADDRESS_1 = 0xA2;
constexpr uint8_t PROM_READ_ADDRESS_2 = 0xA4;
constexpr uint8_t PROM_READ_ADDRESS_3 = 0xA6;
constexpr uint8_t PROM_READ_ADDRESS_4 = 0xA8;
constexpr uint8_t PROM_READ_ADDRESS_5 = 0xAA;
constexpr uint8_t PROM_READ_ADDRESS_6 = 0xAC;
constexpr uint8_t PROM_READ_ADDRESS_7 = 0xAE;

// digital pressure/temperature conversion commands at different sampling rates.
constexpr uint8_t CONVERT_D1_OSR_256 = 0x40;
constexpr uint8_t CONVERT_D1_OSR_512 = 0x42;
constexpr uint8_t CONVERT_D1_OSR_1024 = 0x44;
constexpr uint8_t CONVERT_D1_OSR_2048 = 0x46;
constexpr uint8_t CONVERT_D1_OSR_4096 = 0x48; //used
constexpr uint8_t CONVERT_D2_OSR_256 = 0x50;
constexpr uint8_t CONVERT_D2_OSR_512  = 0x52;
constexpr uint8_t CONVERT_D2_OSR_1024 = 0x54;
constexpr uint8_t CONVERT_D2_OSR_2048 = 0x56;
constexpr uint8_t CONVERT_D2_OSR_4096 = 0x58; //used

//Read ADC
constexpr uint8_t ADC_READ = 0x0;

//additional constants
constexpr uint8_t PROM_LENGTH = 8;
constexpr uint8_t MAX_LENGTH = 6;

