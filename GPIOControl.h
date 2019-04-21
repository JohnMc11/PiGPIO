#pragma once

/* TODO:
  Comment code
*/

/*
  Written by Michael Collins
  Created at 10:39 on 2018-07-08

  Class to control a given GPIO pin

  --- Functions
    --- Public
      setPinOutput(isOutput, isPWM) - Set pin to output if isOutput == true and set to pwm if isPWM == true
      setPinInput(isInput)  - Set pin to input if isInput == true
      getInputValue(&value) - Set value to the value stored in the pin's input file
      getGPIONum()  - Return the GPIO number
      updatePWM() - Updates PWM
      getPWMMax() - Return the max pwm value

    --- Private
      exportGPIO_()  - export GPIO pin for control
      unexportGPIO_()  - unexport GPIO pin
      setDirection_(direction) - Set the direction of the pin (input/output)
      setValueON_OFF_(on) - Turn the pin on and off
*/


#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>  // portable time
#include <sys/timeb.h> // portable time
#include <chrono>

class GPIOControl
{
public:
  GPIOControl();
  GPIOControl(int GPIONum);
  ~GPIOControl();

  int open();
  int close();
  int set(unsigned int value);
  int setPinModeOutput(bool pwm = false);
  int setPinModeInput();
  int getInputValue();
  int getGPIONum();
  int update();

  unsigned int getPWMMax();
  std::string getFormattedInformation();

private:
  int GPIONum_;
  int value_ = 0;
  bool binValue_ = false;
  unsigned long pwmLastMicroCount_ = 0;
  unsigned long pwmIncrement_ = 250;
  unsigned int pwmTickCount_ = 0;
  unsigned int pwmMax_ = 1023;  // 512 works best
  bool inputPin_ = false;
  bool outputPin_ = false;
  bool pwmPin_ = false;
  bool isExported_ = false;
  bool isDirectionSet_ = false;

  std::string controlInfoString_;

// --
  int exportGPIO_();
  int unexportGPIO_();
  int setDirection_();
  int setValueON_OFF_(bool on);
  void addControlInfo_(std::string infoName, std::string infoValue);

  int getMilliCount_();
  long long getMicroCount_();
};
