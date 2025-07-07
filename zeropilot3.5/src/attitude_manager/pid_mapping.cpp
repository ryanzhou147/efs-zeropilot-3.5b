#include "PID_mapping.hpp"

PIDMapping::PIDMapping():
    rollPID(roll_Kp, roll_Ki, roll_Kd,
        roll_tau, output_min, output_max,
        roll_integral_min_lim, roll_integral_max_lim, AM_MAIN_DELAY),
      
    pitchPID(pitch_Kp, pitch_Ki, pitch_Kd,
        pitch_tau, output_min, output_max,
        pitch_integral_min_lim, pitch_integral_max_lim, AM_MAIN_DELAY)
{
    rollPID.PIDInitState();
    pitchPID.PIDInitState();
}

RCMotorControlMessage_t PIDMapping::runControl(RCMotorControlMessage_t controlInputs){
    float roll_setpoint = controlInputs.roll;
    float pitch_setpoint = controlInputs.pitch;

    // Get from sensor fusion -- make sure to update include files when we get this
    float roll_measured = 50.0;
    float pitch_measured = 50.0;

    // Currently, roll & pitch outputs receive absolute roll & pitch angles, not relative to current position.
    float roll_output = rollPID.PIDOutput(roll_setpoint, roll_measured);
    float pitch_output = pitchPID.PIDOutput(pitch_setpoint, pitch_measured);

    controlInputs.roll = roll_output; // setting desired roll angle
    controlInputs.pitch = pitch_output; // setting desired pitch angle

    return controlInputs;
}

