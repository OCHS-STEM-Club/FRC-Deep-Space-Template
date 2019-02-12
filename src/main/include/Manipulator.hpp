#pragma once

#include <iostream>
#include <frc/WPILib.h>
#include <frc/Joystick.h>
#include <ctre/Phoenix.h>
#include <frc/XboxController.h>


class ManipulatorManager {
    private:
    frc::Joystick *stick; 
    frc::XboxController *xbox;

    frc::AnalogPotentiometer *potentiometer;
    frc::DigitalInput *hallEffect;

    WPI_TalonSRX *armMotor; 
    WPI_TalonSRX *extendMotor;
    WPI_TalonSRX *handMotor;

    //double *pidControl;
    //frc::PIDController *pid;

    double *armSpeed;
    double *extendSpeed;

    double *startingAngle;
    double *potDegrees;
    double *caculatedAngle;

    public:
    ManipulatorManager();
    void manipulate();
};