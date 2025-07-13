#pragma once

typedef struct {
    float altitude;         // m
    float rateOfClimb;      // m/s
    long double latitude;   // Decimal degrees
    float latitudeSpeed;    // m/s
    long double longitude;  // Decimal degrees
    float longitudeSpeed;   // m/s
} InputSF;

typedef struct {
    uint8_t roll            // %
    uint8_t pitch           // %
    uint8_t throttle        // %
} OutputFBWA;

typedef struct {
    uint8_t roll            // %
    uint8_t pitch           // %
    uint8_t throttle        // %
    uint8_t altitude        // m
} OutputFBWB;