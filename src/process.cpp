#include "process.h"

#include <unistd.h>

#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_{pid} {};

// DONE: Return this process's ID
int Process::Pid() const {
  if (!pid_) {
    throw "Process pid is missing";
  }
  return pid_;
}

// DONE: Return this process's CPU utilization
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
float Process::CpuUtilization() const {
  long HERTZ{sysconf(_SC_CLK_TCK)};
  return static_cast<float>(LinuxParser::ActiveJiffies(Pid())) / HERTZ /
         UpTime();
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// DONE: Return this process's memory utilization
string Process::Ram() {
  float ram_kb{std::stof(LinuxParser::Ram(Pid()))};
  return FloatToDecimal(ram_kb / 1024, 2);
}

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() const {
  long HERTZ{sysconf(_SC_CLK_TCK)};
  return LinuxParser::UpTime() - LinuxParser::UpTime(Pid()) / HERTZ;
}

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return CpuUtilization() > a.CpuUtilization();
}

// got idea from chatgpt, need a way to represent floats nicely in a string
string FloatToDecimal(float value, int decimals) {
  std::ostringstream out;
  out << std::fixed << std::setprecision(decimals)
      << std::round(value * std::pow(10.0, decimals)) /
             std::pow(10.0, decimals);
  return out.str();
}