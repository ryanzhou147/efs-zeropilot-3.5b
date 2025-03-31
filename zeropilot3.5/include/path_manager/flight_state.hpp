#pragma once
#include "path_manager.hpp"

class FlightState {
    public:
        virtual void execute(PathManager* pathMgr) = 0;
};

class DisarmedState : public FlightState {
    private:
        DisarmedState();
    public:
        void execute(PathManager* pathMgr);
};

class TakeoffState : public FlightState {
    private:
        Waypoint target; //necessary? how do TM commands work for takeoff/landing
        double takeoffSpeed = 0; //speed OR angle is set, depending on what is given
        double takeoffAngle = 0;
        double targetAltitude;

        TakeoffState();
    public:
        void execute(PathManager* pathMgr);
};

class CruisingState : public FlightState {
    private:
        Waypoint target;
        double cruisingSpeed;
        double cruisingAltitude;

        CruisingState();
    public:
        void execute(PathManager* pathMgr);
};

class LoiteringState : public FlightState {
    private:
        Waypoint center;
        double loiteringSpeed;
        double radius;

        LoiteringState();
    public:
        void execute(PathManager* pathMgr);
};

class LandingFlareState : public FlightState {
    private:
        Waypoint target; //necessary??? like no right
        Waypoint flareCoordinate;
        double descentSpeed; //speed OR angle is set, depending on what is given
        double descentAngle;

        LandingFlareState();
    public:
        void execute(PathManager* pathMgr);
};

class LandingRollState : public FlightState {
    private:
        Waypoint target; //necessary??? like no right
        double flareAltitude;

        LandingRollState();
    public:
        void execute(PathManager* pathMgr);
};