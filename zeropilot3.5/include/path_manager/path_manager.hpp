#pragma once
//#include <cstdint>
#include "flight_state.hpp"

struct Waypoint {
    //uint8_t waypoint_id;
    int seq_num;
    double longitude;
    double latitude;
    double altitude;
};

enum PathManagerStatus {COMPLETED_CYCLE = 0, IN_CYCLE, FAILURE_MODE};

class PathManager {
    private:
        FlightState* currentState; //takeoff, cruising, landing, etc.
        PathManagerStatus cycleStatus;
        double cruisingSpeed;
    public:
        PathManager(double cruisingSpeed);
        void execute();

        void setState(FlightState& newState);
        inline FlightState* getState() const {return currentState;};

        PathManagerStatus getStatus() {return cycleStatus;}
        
};