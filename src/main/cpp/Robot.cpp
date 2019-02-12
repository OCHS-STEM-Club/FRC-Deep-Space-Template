
/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Robot.h"
#include <iostream>
#include <frc/SmartDashboard/SmartDashboard.h>
#include "Drive.hpp"
#include "Vision.hpp"
#include "Lift.hpp"
#include "Manipulator.hpp"
#include <ctre/Phoenix.h> 
#include <frc/Joystick.h>
#include <frc/I2C.h>

typedef unsigned char byte;

int step = 0;
int autoNum = 0;

Robot::Robot() {
  driveManager = new DriveManager();
  pixyManager = new PixyManager();
  liftManager = new LiftManager();
  manipulatorManager = new ManipulatorManager();
}

frc::Joystick *stick;

void Robot::RobotInit() {
  m_chooser.AddDefault(kAutoNameDefault, kAutoNameDefault);
  m_chooser.AddObject(kAutoNameCustom, kAutoNameCustom);
  frc::SmartDashboard::PutData("Auto Modes", &m_chooser);

  stick = new frc::Joystick{ 0 };

}



/**
 * This function is called every robot packet, no matter the mode. Use
 * this for items like diagnostics that you want ran during disabled,
 * autonomous, teleoperated and test.
 *
 * <p> This runs after the mode specific periodic functions, but before
 * LiveWindow and SmartDashboard integrated updating.
 */
void Robot::RobotPeriodic() {}

/**
 * This autonomous (along with the chooser code above) shows how to select
 * between different autonomous modes using the dashboard. The sendable chooser
 * code works with the Java SmartDashboard. If you prefer the LabVIEW Dashboard,
 * remove all of the chooser code and uncomment the GetString line to get the
 * auto name from the text box below the Gyro.
 *
 * You can add additional auto modes by adding additional comparisons to the
 * if-else structure below with additional strings. If using the SendableChooser
 * make sure to add them to the chooser code above as well.
 */

void Robot::AutonomousInit() {
  m_autoSelected = m_chooser.GetSelected();
  // m_autoSelected = SmartDashboard::GetString(
  //     "Auto Selector", kAutoNameDefault);
  std::cout << "Auto selected: " << m_autoSelected << std::endl;

  if (m_autoSelected == kAutoNameCustom) {
    // Custom Auto goes here
  } else {
    // Default Auto goes here
  }

driveManager->reset();
}

void Robot::AutonomousPeriodic() {
  if (m_autoSelected == kAutoNameCustom) {
    // Custom Auto goes here
  } else {
    // Default Auto goes here
  }

 /*if (autoNum = 0) {
    switch(step) {
      case 0: driveManager->turn(90);
        break;
    } 
 } */
 driveManager->turn(180);
}

void Robot::TeleopInit() {
}

void Robot::TeleopPeriodic() {
  //driveManager->driveTrain();
  pixyManager->pixy();

  if (stick->GetRawButton(12)) {
    pixyManager->pixyFunct();
  }
  else {
    driveManager->driveTrain();
  } 

  liftManager->Lift();

  manipulatorManager->manipulate();
}

void Robot::TestPeriodic() {
  
}

#ifndef RUNNING_FRC_TESTS
START_ROBOT_CLASS(Robot)
#endif