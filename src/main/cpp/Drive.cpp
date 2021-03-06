#include "Drive.hpp"

#include <Robot.h>

DriveManager::DriveManager () {
    stick = new frc::Joystick{ 0 };

    //driveMotorFrontLeft = new WPI_TalonSRX(1);
    //driveMotorFrontRight = new WPI_TalonSRX(3);
    //driveMotorBackLeft = new WPI_TalonSRX(2);
    //driveMotorBackRight = new WPI_TalonSRX(4);

    driveMotorFrontLeft = new rev::CANSparkMax(1, rev::CANSparkMax::MotorType::kBrushless);
    driveMotorFrontRight = new rev::CANSparkMax(2, rev::CANSparkMax::MotorType::kBrushless);
    driveMotorBackLeft = new rev::CANSparkMax(3, rev::CANSparkMax::MotorType::kBrushless);
    driveMotorBackRight = new rev::CANSparkMax(4, rev::CANSparkMax::MotorType::kBrushless);


    //encoders for CANSparkMax
    encFrontLeft = new rev::CANEncoder(*driveMotorFrontLeft);
    encFrontRight = new rev::CANEncoder(*driveMotorFrontRight);
    encBackLeft = new rev::CANEncoder(*driveMotorBackLeft);
    encBackRight = new rev::CANEncoder(*driveMotorBackRight);

    mecanumDrive = new frc::MecanumDrive(*driveMotorFrontLeft, *driveMotorBackLeft, *driveMotorFrontRight, *driveMotorBackRight);

    //frontLeftPID = new frc::PIDController(0.0, 0.0, 0.0, &encFrontLeft, &driveMotorFrontLeft);

    //Gyro
    try {
        ahrs = new AHRS(SPI::Port::kMXP);
    }
    catch(std::exception ex) {
        std::string err_string = "Error initalizing naxX-MXP"; 
        err_string += ex.what();
        DriverStation::ReportError(err_string.c_str());
    }
    ahrs->Reset(); 

    time = new frc::Timer;

    //Joystick values
    xStickValue = new double; 
    yStickValue = new double; 
    zStickValue = new double; 
    
    driveGyro = new double;
    gyro = new double; 
    error = new double;

    p = new double;
    i = new double; 
    integral = new double;
    d = new double; 
    prevError = new double;

    *p = 0.0095;
    *i = 0;
    *d = 0;
    *integral = 0;
    *prevError = 0;

    driveToggle = new bool;  
    driveLatch = new bool; 
    *driveToggle = true;
    *driveLatch = false;

    idleModeToggle = new bool;
    idleModeLatch = new bool;
    *idleModeToggle = false;  //true for brake
    *idleModeLatch = false; 

    revolutions = 0;

}

int Sign(double input) {
    if (input > 0) {
        return 1;
    }
    else if (input < 0) {
        return -1;
    }
    else if (input == 0) {
        return 0;
    }
}

double deadband(double joystickValue, double deadbandValue) {
    if(abs(joystickValue) < 0.2){
        return 0;
    }
    else{
        return (1 / (1 - deadbandValue)) * (joystickValue + (-Sign(joystickValue) * deadbandValue));
    } 
}

void DriveManager::driveTrain() {

    //*xStickValue = -stick->GetRawAxis(0);
    //*yStickValue = -stick->GetRawAxis(1);
    //*zStickValue = stick->GetRawAxis(2);


        /*if (abs(stick->GetRawAxis(1)) < .2) {
			*xStickValue = 0;
		}
		else {
			*xStickValue = -stick->GetRawAxis(1);
		}*/
        *xStickValue = deadband(-stick->GetRawAxis(1), 0.2);

		//Repeat of above for Y
		/*if (-stick->GetRawAxis(0) < -0.1 and -stick->GetRawAxis(0) > 0.4)
		{
			*yStickValue = 0;
		}
		else
		{
			*yStickValue = stick->GetRawAxis(0);
		}*/
        *yStickValue = deadband(stick->GetRawAxis(0), 0.2);

		//Repeat of above for Z
		/*if (stick->GetRawAxis(2) < .1 && stick->GetRawAxis(2) > -0.15)
		{
			*zStickValue = 0;
		}
		else
		{
			*zStickValue = stick->GetRawAxis(2);
		}*/
        *zStickValue = deadband(stick->GetRawAxis(2), 0.125);

        /*xStickRawValue = stick->GetRawAxis(1);
        if(abs(xStickRawValue) < 0.2){
            *xStickValue = 0;
        }
        else{
            xStickRawValue = (1/(1-0.2))*(xStickRawValue+(-Sign(xStickRawValue) * 0.2));
        } */


        if (stick->GetRawButton(1)) {
            *xStickValue = *xStickValue * 0.3;
            *yStickValue = *yStickValue * 0.5;
            *zStickValue = *zStickValue * 0.3;
        }

    frc::SmartDashboard::PutNumber("joystickY", stick->GetRawAxis(0));
    frc::SmartDashboard::PutNumber("joystickx", stick->GetRawAxis(1));


    if (stick->GetRawButton(6) and !*driveLatch) {
        *driveToggle = !*driveToggle;
        *driveLatch = true;
    }
    else if (!stick->GetRawButton(6) and *driveLatch) {
        *driveLatch = false;
    }

    if (*driveToggle) {
        *driveGyro = 0;
    }
    else if (!*driveToggle) {
        *driveGyro = ahrs->GetAngle();
    }

    frc::SmartDashboard::PutNumber("driveGyro", *driveGyro);

    //if (!stick->GetRawButton(11)){  // && !stick->GetRawButton(9)) {
        mecanumDrive->DriveCartesian(*yStickValue, *xStickValue, *zStickValue, *driveGyro);
    //}

    *gyro = ahrs->GetAngle(); 
    frc::SmartDashboard::PutNumber("gyro", *gyro);

    if (stick->GetRawButton(4)) {
        ahrs->Reset();
    }

    //velocities for CANSparkMax
    frc::SmartDashboard::PutNumber("velocityFrontLeft", encFrontLeft->GetVelocity());
    frc::SmartDashboard::PutNumber("velocityFrontRight", encFrontRight->GetVelocity());
    frc::SmartDashboard::PutNumber("velocityFBackLeft", encBackLeft->GetVelocity());
    frc::SmartDashboard::PutNumber("velocityBackRight", encBackRight->GetVelocity());

    //motor temp for CANSparkMax only
    frc::SmartDashboard::PutNumber("tempFrontLeft", driveMotorFrontLeft->GetMotorTemperature());
    frc::SmartDashboard::PutNumber("tempFrontRight", driveMotorFrontRight->GetMotorTemperature());
    frc::SmartDashboard::PutNumber("tempBackLeft", driveMotorBackLeft->GetMotorTemperature());
    frc::SmartDashboard::PutNumber("tempBackRight", driveMotorBackRight->GetMotorTemperature());

    //current and voltage
    frc::SmartDashboard::PutNumber("currentFrontLeft", driveMotorFrontLeft->GetOutputCurrent());
    frc::SmartDashboard::PutNumber("currentFrontRight", driveMotorFrontRight->GetOutputCurrent());
    frc::SmartDashboard::PutNumber("currentBackLeft", driveMotorBackLeft->GetOutputCurrent());
    frc::SmartDashboard::PutNumber("currentBackRight", driveMotorBackRight->GetOutputCurrent());

    frc::SmartDashboard::PutNumber("voltageFrontLeft", driveMotorFrontLeft->GetBusVoltage());
    frc::SmartDashboard::PutNumber("voltageFrontRight", driveMotorFrontRight->GetBusVoltage());
    frc::SmartDashboard::PutNumber("voltageBackLeft", driveMotorBackLeft->GetBusVoltage());
    frc::SmartDashboard::PutNumber("voltageBackRight", driveMotorBackRight->GetBusVoltage());

    if (stick->GetRawButton(10) and !*idleModeLatch) {
        *idleModeToggle = !*idleModeToggle;
        *idleModeLatch = true;
    }
    else if (!stick->GetRawButton(10) and *idleModeLatch) {
        *idleModeLatch = false;
    }

    if (*idleModeToggle) {
        driveMotorFrontLeft->SetIdleMode(rev::CANSparkMax::IdleMode::kBrake);
        driveMotorFrontRight->SetIdleMode(rev::CANSparkMax::IdleMode::kBrake);
        driveMotorBackLeft->SetIdleMode(rev::CANSparkMax::IdleMode::kBrake);
        driveMotorBackRight->SetIdleMode(rev::CANSparkMax::IdleMode::kBrake);
        frc::SmartDashboard::PutString("driveMotorIdleMode", "brake");
    }
    else if (!*idleModeToggle) {
        driveMotorFrontLeft->SetIdleMode(rev::CANSparkMax::IdleMode::kCoast);
        driveMotorFrontRight->SetIdleMode(rev::CANSparkMax::IdleMode::kCoast);
        driveMotorBackLeft->SetIdleMode(rev::CANSparkMax::IdleMode::kCoast);
        driveMotorBackRight->SetIdleMode(rev::CANSparkMax::IdleMode::kCoast);
        frc::SmartDashboard::PutString("driveMotorIdleMode", "coast");
    }


    angle = ahrs->GetAngle();
  
    if (revolutions >= 1) {
        angle = angle - (revolutions * 360);
    }
    
    if (revolutions <= 0) {
        angle = angle + abs(revolutions * 360);
    }

    if ((angle > 360) or (angle < 0)) {
        if (angle > 360) {
        revolutions++;
        }

        if (angle < 360) {
        revolutions--;
        }

        angle = ahrs->GetAngle();

        if (revolutions >= 1) {
        angle = angle - (revolutions * 360);
        }
    
        if (revolutions <= 0) {
        angle = angle + abs(revolutions * 360);
        }
    }
    frc::SmartDashboard::PutNumber("drive gyro", angle);

    /*if (stick->GetRawButton(11)) {
        if ((angle < 135) && (angle > 45)) {
            turnWant = 90;
        }
        else if ((angle > 315) && (angle < 360)) {
            turnWant = 360;
        }
        else if ((angle > 0) && (angle < 45)) {
            turnWant = 0;
        }
        else if ((angle > 135) && (angle < 225)) {
            turnWant = 180; 
        }
        else if ((angle > 225) && (angle < 315)) {
            turnWant = 270;
        }


        turnOffset = turnWant - angle; 
        turnCorrection = (1.0 * turnOffset/90) * 1.0;

        /*frc::SmartDashboard::PutNumber("turnOffset", turnOffset);
        if (abs(turnOffset) < PIXY_DEADBAND_TURN) {
            turnCorrection = 0;
        } */

        //driveManager->control(turnCorrection , 0, 0, true);
        //mecanumDrive->DriveCartesian(*yStickValue, *xStickValue, turnCorrection, *driveGyro);
    //}  
    
    /*if (stick->GetRawButton(9)) {
        if ((angle < 360) and (angle > 270)) {
            turnWant = 299;
        }
        else if ((angle > 0) and (angle < 90)) {
            turnWant = 61;
        }
        else if ((angle > 90) and (angle < 180)) {
            turnWant = 119;
        }
        else if ((angle > 180) and (angle < 270)) {
            turnWant = 241;
        }

        turnOffset = turnWant - angle; 
        turnCorrection = (1.0 * turnOffset/90) * 1.0;

        mecanumDrive->DriveCartesian(*yStickValue, *xStickValue, turnCorrection, *driveGyro);
    } */
}
/*
void DriveManager::control(double turn, double strafe, double drive, bool brake) { 
    mecanumDrive->DriveCartesian(strafe, drive, turn, 0);

    if (brake) {
        driveMotorFrontLeft->SetIdleMode(rev::CANSparkMax::IdleMode::kBrake);
        driveMotorFrontRight->SetIdleMode(rev::CANSparkMax::IdleMode::kBrake);
        driveMotorBackLeft->SetIdleMode(rev::CANSparkMax::IdleMode::kBrake);
        driveMotorBackRight->SetIdleMode(rev::CANSparkMax::IdleMode::kBrake);
        frc::SmartDashboard::PutString("driveMotorIdleMode", "brake");
    }

}

void DriveManager::turn(int angle) {
    double power; 
    double turnP = 0.0095;
   // int want = angle;

    *gyro = ahrs->GetAngle();
    frc::SmartDashboard::PutNumber("gyro", *gyro);

  //  power = (-(*gyro - angle) * turnP);

 //   if (fabs(*gyro - angle) < 1) {
 //       power = 0;
 //   }

    *error = -(*gyro - angle);
    *integral += *error;
    power = (*p * *error) + (*i * *integral) + (*d * *prevError);
    *prevError = *error;

    frc::SmartDashboard::PutNumber("auto power", power);
    mecanumDrive->DriveCartesian(0, 0, power, 0);
}

void DriveManager::reset() {
    ahrs->Reset();
    //*time->Reset();

    *integral = 0;
    *prevError = 0;
} */