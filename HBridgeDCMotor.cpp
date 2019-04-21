#include "HBridgeDCMotor.h"

HBridgeDCMotor::HBridgeDCMotor()
{
}
HBridgeDCMotor::HBridgeDCMotor(int forw, int rev)
{
  setForwRevGPIONum(forw, rev);
  //errorNum_ += startGPIO_();
}
HBridgeDCMotor::HBridgeDCMotor(int forw, int rev, int enable)
{
  setForwRevGPIONum(forw, rev);
  setEnableGPIONum(enable);
  //errorNum_ += startGPIO_();
}

// Destructor
HBridgeDCMotor::~HBridgeDCMotor()
{
  // Destroy any pointers
}

// -- Public Functions
int HBridgeDCMotor::update()
{
  if (!GPIOStarted_) {
    startGPIO_();
    return 1;
  }

  // Set the appropriate GPIO PWM to PWMMax * val
  int pwmSetVal = 0;
  if (outputVal_ < 0) {
    pwmSetVal = int((outputVal_) * double(reverseGPIO_.getPWMMax()));
    reverseGPIO_.set(std::abs(pwmSetVal));
    forwardGPIO_.set(0);
    //std::cout << "PWMVal:" << pwmSetVal << "\n";
  } else if (outputVal_ > 0) {
    pwmSetVal = int((outputVal_) * double(forwardGPIO_.getPWMMax()));
    forwardGPIO_.set(pwmSetVal);
    reverseGPIO_.set(0);
    //std::cout << "PWMVal:" << pwmSetVal << "\n";
  } else if (outputVal_ == 0) {
    // Set both GPIO to 0
    forwardGPIO_.set(0);
    reverseGPIO_.set(0);
  }

  errorNum_ += forwardGPIO_.update();
  errorNum_ += reverseGPIO_.update();
  if (usingEnable_) errorNum_ += enableGPIO_.update();

  return errorNum_;
}

void HBridgeDCMotor::set(double val)
{
  if (val > 1.0) val = 1.0;
  if (val < -1.0) val = -1.0;

  outputVal_ = val;
}

void HBridgeDCMotor::setForwRevGPIONum(int forw, int rev)
{
  // Set the GPIOControl variables
  GPIOControl tmpForward(forw);
  GPIOControl tmpReverse(rev);

  forwardGPIO_ = tmpForward;
  reverseGPIO_ = tmpReverse;
}

void HBridgeDCMotor::setEnableGPIONum(int enable)
{
  GPIOControl tmpEnable(enable);
  enableGPIO_ = tmpEnable;

  usingEnable_ = true;
}

int HBridgeDCMotor::close()
{
  errorNum_ += forwardGPIO_.close();
  errorNum_ += errorNum_ += reverseGPIO_.close();
  errorNum_ += enableGPIO_.close();

  return errorNum_;
}

std::string HBridgeDCMotor::getFormattedInformation()
{
  controlInfoString_ = "";
  addControlInfo_("forwardGPIO", forwardGPIO_.getFormattedInformation());
  addControlInfo_("reverseGPIO", reverseGPIO_.getFormattedInformation());
  addControlInfo_("enableGPIO", enableGPIO_.getFormattedInformation());
  addControlInfo_("usingEnable", std::to_string((int)usingEnable_));
  //addControlInfo_("GPIOStarted", std::to_string((int)GPIOStarted_));
  addControlInfo_("outputVal", std::to_string(outputVal_));
  return controlInfoString_;
}

// -- Private Functions
int HBridgeDCMotor::startGPIO_()
{
  errorNum_ += forwardGPIO_.open();
  errorNum_ += reverseGPIO_.open();
  errorNum_ += forwardGPIO_.setPinModeOutput(true);
  errorNum_ += reverseGPIO_.setPinModeOutput(true);
  if (usingEnable_) errorNum_ += enableGPIO_.open();
  if (usingEnable_) errorNum_ += enableGPIO_.setPinModeOutput(false);

  GPIOStarted_ = true;

  return errorNum_;
}

void HBridgeDCMotor::addControlInfo_(std::string infoName, std::string infoValue)
{
  controlInfoString_ += "<" + infoName + ">" + infoValue + "</" + infoName + ">";
}
