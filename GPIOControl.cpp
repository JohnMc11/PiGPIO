#include "GPIOControl.h"

// Default constructor
GPIOControl::GPIOControl()
{
  GPIONum_ = -1;
}
// Constructor
GPIOControl::GPIOControl(int GPIONum)
{
  GPIONum_ = GPIONum;
  //exportGPIO_();
}

// Destructor
GPIOControl::~GPIOControl()
{
  // Destroy any pointers here
}

// -- Public functions
int GPIOControl::open()
{
  return exportGPIO_();
}
int GPIOControl::close()
{
  return unexportGPIO_();
}

int GPIOControl::set(unsigned int value)
{
  value_ = value;
  /*if (pwmPin_) {
    // pwm function
  } else if (outputPin_) {
    if (value == 0) return setValueON_OFF_(false);
    else if (value > 0) return setValueON_OFF_(true);
  }
  return 2; // Return 2 if nothing happened
  */
  return 0;
}

int GPIOControl::setPinModeOutput(bool pwm)
{
  inputPin_ = false;
  outputPin_ = true;
  pwmPin_ = pwm;
  return 0;
  //return setDirection_();
}

int GPIOControl::setPinModeInput()
{
  inputPin_ = true;
  outputPin_ = false;
  pwmPin_ = false;
  return 0;
  //return setDirection_();
}

int GPIOControl::getInputValue()
{
  if (GPIONum_ < 0) return 1;
  std::string valuePath = "/sys/class/gpio/gpio" + std::to_string(GPIONum_) + "/value";
  std::ifstream valueStream(valuePath.c_str());
  if (valueStream.fail()) {
    return -1;
  }

  std::string valueStr;
  valueStream >> valueStr;
  valueStream.close();

  value_ = std::stoi(valueStr);
  return value_;
}

int GPIOControl::getGPIONum()
{
  return GPIONum_;
}

int GPIOControl::update()
{
  if (GPIONum_ < 0) return 1;
  // Direction
  if (!isDirectionSet_) {
    setDirection_();
  }

  // Output
  int errorTotal = 0;
  // Non PWM
  if (!pwmPin_) {
    if (value_ <= 0) errorTotal += setValueON_OFF_(false);
    else errorTotal += setValueON_OFF_(true);
    return errorTotal;
  }

  // PWM
  unsigned long currentMicroCount = getMicroCount_();
  if (currentMicroCount - pwmLastMicroCount_ >= pwmIncrement_) pwmTickCount_++; // Increment the tick count

  if (binValue_) {
    if (pwmTickCount_ >= value_) {
      errorTotal += setValueON_OFF_(false);
    }
  } else {
    if (pwmTickCount_ >= pwmMax_) {
      if (value_ > 0) errorTotal += setValueON_OFF_(true);
      pwmTickCount_ = 0;
    }
  }

  return errorTotal;
}

unsigned int GPIOControl::getPWMMax()
{
  return pwmMax_;
}

std::string GPIOControl::getFormattedInformation()
{
  // Construct the information string
  controlInfoString_ = "";
  addControlInfo_("GPIONum", std::to_string(GPIONum_));
  addControlInfo_("value", std::to_string(value_));
  //addControlInfo_("binValue", std::to_string(binValue_));
  //addControlInfo_("pwmLastMicroCount", std::to_string(pwmLastMicroCount_));
  addControlInfo_("pwmIncrement", std::to_string(pwmIncrement_));
  //addControlInfo_("pwmTickCount", std::to_string(pwmTickCount_));
  addControlInfo_("pwmMax", std::to_string(pwmMax_));
  addControlInfo_("inputPin", std::to_string((int)inputPin_));
  addControlInfo_("outputPin", std::to_string((int)outputPin_));
  addControlInfo_("pwmPin", std::to_string((int)pwmPin_));
  //addControlInfo_("isExported", std::to_string((int)isExported_));
  //addControlInfo_("isDirectionSet", std::to_string((int)isDirectionSet_));

  return controlInfoString_;
}

// -- Private functions
int GPIOControl::exportGPIO_()
{
  if (GPIONum_ < 0) return 1;

  std::string exportPath = "/sys/class/gpio/export";
  std::ofstream exportStream(exportPath.c_str());
  if (exportStream.fail()) {
    return -1;
  }

  exportStream << GPIONum_;
  exportStream.close();

  int start = getMilliCount_();
  while(getMilliCount_() - start < 50);
  isExported_ = true;

  return 0;
}

int GPIOControl::unexportGPIO_()
{
  if (GPIONum_ < 0) return 1;

  std::string unexportPath = "/sys/class/gpio/unexport";
  std::ofstream unexportStream(unexportPath.c_str());
  if (unexportStream.fail()) {
    return -1;
  }

  unexportStream << GPIONum_;
  unexportStream.close();
  return 0;
}

int GPIOControl::setDirection_()
{
  if (GPIONum_ < 0) return 1;

  std::string directionPath = "/sys/class/gpio/gpio" + std::to_string(GPIONum_) + "/direction";
  std::ofstream directionStream(directionPath.c_str());
  if (directionStream.fail()) {
    std::cout << "Error on set direction\n";
    return -1;
  }

  if (inputPin_ && outputPin_) return -2;
  if (!inputPin_ && !outputPin_) return -3;
  if (inputPin_) {
    directionStream << "in";
  } else if (outputPin_) {
    directionStream << "out";
  }
  directionStream.close();
  isDirectionSet_ = true;

  std::cout << "GPIO" << GPIONum_ << " set direction.\n";

  return 0;
}

int GPIOControl::setValueON_OFF_(bool on)
{
  if (GPIONum_ < 0) return 1;
  if (!isExported_) return 0;

  if (on == binValue_) return 0;  // Only update the output if it is different from what it already is

  std::string valuePath = "/sys/class/gpio/gpio" + std::to_string(GPIONum_) + "/value";
  std::ofstream valueStream(valuePath.c_str());
  if (valueStream.fail()) {
    return -1;
  }

  binValue_ = on;

  if (on) {
    valueStream << "1";
  } else {
    valueStream << "0";
  }
  valueStream.close();
  return 0;
}

void GPIOControl::addControlInfo_(std::string infoName, std::string infoValue)
{
  //controlInfoString_ += infoName + ":" + infoValue + "\n";
  controlInfoString_ += "<" + infoName + ">" + infoValue + "</" + infoName + ">";
}

// -- Time
int GPIOControl::getMilliCount_()
{
  timeb tb;
	ftime(&tb);
	int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
	return nCount;
}

long long GPIOControl::getMicroCount_()
{
  auto now = std::chrono::system_clock::now();
  auto now_mc = std::chrono::time_point_cast<std::chrono::microseconds>(now);
  auto epoch = now_mc.time_since_epoch();
  //auto value = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
  return (long long)epoch.count();
}
