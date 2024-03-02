#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <cmath>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line, key;
  float value;
  float mem_total(0);
  float mem_free(0);

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        mem_total = value;
      } else if (key == "MemFree:") {
        mem_free = value;
      }
      if (mem_total && mem_free) {
        return (mem_total - mem_free) / mem_total;
      }
    }
  }
  return 0;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  long uptime;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return uptime;
  }
  return 0;
}

// DONE: Read and return the number of jiffies for the system
// NOTE: I think it should be a part of the Processor class from encapsulation
// perspective, since it's a calculated processor property and it's calculated
// from other processor properties
long LinuxParser::Jiffies() {
  vector<string> jiffies{CpuUtilization()};
  long total(0);

  if (jiffies.size() > 0) {
    // last active jiffie index is 7. jiffies with index 8, 9 are included in 0
    // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
    for (int i = 0; i <= 7; i++) {
      total += stol(jiffies[i]);
    }
  }
  return total;
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);

    vector<string> pid_stat_vector{StringstreamToVector(linestream)};

    long total_active_jiffies{0};

    const vector<int> active_jiffies_indexes{13, 14, 15, 16};

    for (int jiffie_index : active_jiffies_indexes) {
      total_active_jiffies += stol(pid_stat_vector[jiffie_index]);
    }
    return total_active_jiffies;
  }
  return 0;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return Jiffies() - IdleJiffies(); }

// DONE: Read and return the number of idle jiffies for the system
// NOTE: I think it should be a part of the Processor class from encapsulation
// perspective, since it's a calculated processor property and it's calculated
// from other processor properties
long LinuxParser::IdleJiffies() {
  vector<string> jiffies{CpuUtilization()};
  if (jiffies.size() > 0) {
    long idle{stol(jiffies[3])};
    long iowait{stol(jiffies[4])};
    return idle + iowait;
  }
  return 0;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  auto cpu_linestream{FileKeyLinestream(kProcDirectory + kStatFilename, "cpu")};
  vector<string> cpu_utilization;

  if (cpu_linestream.has_value()) {
    cpu_utilization = StringstreamToVector(cpu_linestream.value());
    // remove "cpu" key
    cpu_utilization.erase(cpu_utilization.begin());
  }

  return cpu_utilization;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return FileKeyFirstValue(kProcDirectory + kStatFilename, "processes");
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  return FileKeyFirstValue(kProcDirectory + kStatFilename, "procs_running");
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream{kProcDirectory + to_string(pid) + kCmdlineFilename};
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
  return line;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string key, value;
  auto filestream{FileKeyLinestream(
      kProcDirectory + to_string(pid) + kStatusFilename, "VmSize:")};
  if (filestream.has_value()) {
    filestream.value() >> key >> value;
  }
  return value;
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string key, value;
  auto filestream{FileKeyLinestream(
      kProcDirectory + to_string(pid) + kStatusFilename, "Uid:")};
  if (filestream.has_value()) {
    filestream.value() >> key >> value;
  }
  return value;
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line, line_user, line_x, line_uid;
  string uid{Uid(pid)};

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);

      linestream >> line_user >> line_x >> line_uid;
      if (line_uid == uid) {
        return line_user;
      }
    }
  }
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  std::ifstream filestream{kProcDirectory + to_string(pid) + kStatFilename};
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    vector<string> values{StringstreamToVector(linestream)};
    return stol(values[21]);
  }
  return 0;
}

// utilities

vector<string> LinuxParser::StringstreamToVector(
    std::istringstream& stringstream) {
  string token;
  vector<string> tokens{};
  while (stringstream >> token) {
    tokens.push_back(token);
  }
  return tokens;
}

std::optional<std::istringstream> LinuxParser::FileKeyLinestream(
    string filename, string key) {
  string line;
  std::ifstream filestream(filename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.substr(0, key.length()) == key) {
        std::istringstream linestream(line);
        return std::optional<std::istringstream>(std::move(linestream));
      }
    }
  }
  return std::nullopt;
}

int LinuxParser::FileKeyFirstValue(string filename, string key) {
  string key_ignore;
  int value;
  auto key_linestream{FileKeyLinestream(filename, key)};
  if (key_linestream.has_value()) {
    key_linestream.value() >> key_ignore >> value;
    return value;
  }
  return 0;
}
