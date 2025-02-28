#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// DONE: Return the system's CPU
Processor& System::Cpu() {
  cpu_ = Processor();
  return cpu_;
}

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  vector<int> pids{LinuxParser::Pids()};
  processes_ = {};

  for (int pid : pids) {
    processes_.push_back(Process(pid));
  }

  std::sort(
      processes_.begin(), processes_.end(),
      [](const auto& first, const auto& second) { return first < second; });
  return processes_;
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() {
  if (!kernel_cache_.has_value()) {
    kernel_cache_ = LinuxParser::Kernel();
  }
  return kernel_cache_.value();
}

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// DONE: Return the operating system name
std::string System::OperatingSystem() {
  if (!os_cache_.has_value()) {
    os_cache_ = LinuxParser::OperatingSystem();
  }
  return os_cache_.value();
}

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// DONE: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
