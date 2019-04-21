#pragma once
// HBridgeDCMotor.h
/*
  Written by Michael Collins
  Created at 21:55 on 2018-08-03

  Class to control a DC motor with the TI SN75441ONE H-Bridge chip or compatible chips
  Datasheet: http://www.ti.com/lit/ds/symlink/sn754410.pdf

  --- Functions
    --- Public
      update()  - Updates the GPIOControl classes for each GPIO used for the H-Bridge
      set(double) - Set motor to an output percentage (-1.0 to 1.0)
      setForwRevGPIONum(int, int) - Set the GPIO numbers for forward and reverse
      setEnableGPIO(int) - Optionally set the enable GPIO number if dynamically enabling/disabling the motor with the H-Bridge
      release() - unexport the GPIO used for the HBridgeControl

    --- Private
      startGPIO_()  - Export the GPIO pins for the H-Bridge control
*/

#include "GPIOControl.h"

class HBridgeDCMotor
{
public:
  HBridgeDCMotor();
  HBridgeDCMotor(int forw, int rev);
  HBridgeDCMotor(int forw, int rev, int enable);
  ~HBridgeDCMotor();

  int update();
  void set(double val);
  void setForwRevGPIONum(int forw, int rev);
  void setEnableGPIONum(int enable);
  int close();

  std::string getFormattedInformation();

private:
  int startGPIO_();
  void addControlInfo_(std::string infoName, std::string infoValue);

  GPIOControl forwardGPIO_;
  GPIOControl reverseGPIO_;
  GPIOControl enableGPIO_;

  bool usingEnable_ = false;
  bool GPIOStarted_ = false;
  double outputVal_ = 0.0;
  int errorNum_ = 0;

  std::string controlInfoString_;
};
