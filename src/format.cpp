#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int seconds_left = seconds % 60;
  return LeadingZero(hours) + ":" + LeadingZero(minutes) + ":" +
         LeadingZero(seconds_left);
}

string Format::LeadingZero(int value) {
  return value < 10 ? "0" + std::to_string(value) : std::to_string(value);
}

string Format::KbToMb(string value) {
  float float_value{std::stof(value)};
  std::ostringstream stream;
  stream << std::fixed << std::setprecision(2) << float_value / 1024;
  return stream.str();
}