#pragma once
#include <cstdint>

typedef union TMMessageData_u {
  struct{
      int32_t alt;
      int32_t lat;
      int32_t lon;
      int32_t relative_alt;
      int16_t vx;
      int16_t vy;
      int16_t vz;
      uint16_t hdg;
  } GPOSData_t;
  struct{
      uint16_t roll;
      uint16_t pitch;
      uint16_t yaw;
      uint16_t throttle;
      uint16_t flap_angle;
      uint16_t arm;
  } RCData_t;
  struct{
      int16_t temperature;
      uint16_t* voltages;
      int16_t current_battery;
      int32_t current_consumed;
      int32_t energy_consumed;
      int8_t battery_remaining;
      int32_t time_remaining;
      uint8_t charge_state; // 1 = Normal, 2 = Low, 3 = Critical
  } BMData_t;
} TMMessageData_t;

typedef struct{
    enum{
        GPOS_DATA,
        RC_DATA,
        BM_DATA
    } DataType;
    TMMessageData_t tm_message_data;
    uint32_t time_boot_ms = 0;
} TMMessage_t;

inline TMMessage_t GPOSData_Pack(uint32_t time_boot_ms, int32_t alt, int32_t lat, int32_t lon, int32_t relative_alt, int16_t vx, int16_t vy, int16_t vz,uint16_t hdg) {
    const TMMessageData_t data = {.GPOSData_t={alt, lat, lon ,relative_alt, vx, vy, vz, hdg }};
    return TMMessage_t{TMMessage_t::GPOS_DATA, data, time_boot_ms};
}

inline TMMessage_t RCData_Pack(uint32_t time_boot_ms, float roll, float pitch, float yaw, float throttle, float flap_angle, float arm) {
    auto roll_PPM = static_cast<uint16_t>(1000 + roll * 10);
    auto pitch_PPM = static_cast<uint16_t>(1000 + pitch * 10);
    auto yaw_PPM = static_cast<uint16_t>(1000 + yaw * 10);
    auto throttle_PPM = static_cast<uint16_t>(1000 + throttle * 10);
    auto flap_angle_PPM = static_cast<uint16_t>(1000 + flap_angle * 10);
    auto arm_PPM = static_cast<uint16_t>(1000 + arm * 10);
    const TMMessageData_t data = {.RCData_t ={roll_PPM, pitch_PPM, yaw_PPM, throttle_PPM, flap_angle_PPM, arm_PPM }};
    return TMMessage_t{TMMessage_t::RC_DATA, data, time_boot_ms};
}

inline TMMessage_t BMData_Pack(uint32_t time_boot_ms, int16_t temperature, float *voltages, uint8_t voltage_len, int16_t current_battery, int32_t current_consumed,
    int32_t energy_consumed, int8_t battery_remaining, int32_t time_remaining, uint8_t charge_state) {
    uint16_t mavlink_voltage_array[16] = {UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX};
    for (int i = 0; i < voltage_len; i++) {
        mavlink_voltage_array[i] = static_cast<uint16_t>(voltages[i]);
    }
    if (temperature == -1) {
        temperature = INT16_MAX;
    }
    const TMMessageData_t data = {.BMData_t ={temperature, mavlink_voltage_array, current_battery,
    current_consumed, energy_consumed, battery_remaining, time_remaining, charge_state}};
    return TMMessage_t{TMMessage_t::BM_DATA, data, time_boot_ms};
}
