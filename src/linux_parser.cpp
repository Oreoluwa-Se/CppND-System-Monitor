#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

bool isNumber(const string &num) {
  if (num == "")
    return false;
  return num.find_first_not_of("-.0123456789") == string::npos;
}

template <typename T>
T findValueByKey(string const &KeyFilter, const string &filename) {
  std::string line, key;
  T value{};

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream ls(line);
      while (ls >> key >> value) {
        if (key == KeyFilter)
          return value;
      }
    }
  }
  return value;
}
template <typename T> T getValueOfFile(std::string const &filename) {
  std::string line;
  T value{};

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
}

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
        if (key == "PRETTY_NAME=Ubuntu 20.04.3 LTS") {
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
  string os, kernel, version;
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
  DIR *directory = opendir(kProcDirectory.c_str());
  struct dirent *file;
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
  std::string memTotal = "MemTotal:";
  std::string memFree = "MemFree:";
  float total = findValueByKey<float>(memTotal, kMeminfoFilename);
  float free = findValueByKey<float>(memFree, kMeminfoFilename);
  return (total - free) / total;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  auto val = getValueOfFile<string>(kUptimeFilename);
  return std::stol(val);
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> utilized;
  string line, value;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream ls(line);
    ls >> value;
    while (ls >> value) {
      utilized.emplace_back(value);
    }
  }
  return utilized;
}

// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long total{};
  vector<string> jiffs = CpuUtilization();
  for (auto &value : jiffs) {
    if (isNumber(value))
      total += std::stol(value);
  }

  return total;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> jiff = CpuUtilization();
  auto value = jiff[CPUStates::kIdle_];
  long idle = (isNumber(value)) ? std::stol(value) : (long)0;
  value = jiff[CPUStates::kIOwait_];
  long wait = (isNumber(value)) ? std::stol(value) : (long)0;

  return idle + wait;
}

// DONE: Read and return the number of Active jiffies for the system
long LinuxParser::ActiveJiffies() { return Jiffies() - IdleJiffies(); }

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  auto out = findValueByKey<string>("processes", kStatFilename);
  return (isNumber(out)) ? std::stoi(out) : 0;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  auto out = findValueByKey<string>("procs_running", kStatFilename);
  return (isNumber(out)) ? std::stoi(out) : 0;
}

// DONE: Read and return the number of active jiffies for a PID
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
long LinuxParser::ActiveJiffies(int pid) {
  long total_time = 0;
  string line, value;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                       LinuxParser::kStatFilename);
  if (stream.is_open() && std::getline(stream, line)) {
    // int key_loc[] = {14, 15, 16, 17};
    int key_loc[] = {13, 14, 15, 16};
    int idx = 0;
    std::istringstream ls(line);
    while (ls >> value && idx < key_loc[3]) {
      if (idx >= key_loc[0]) {
        if (isNumber(value))
          total_time += std::stol(value);
      }
      idx += 1;
    }
    ls >> value;
    if (isNumber(value))
      total_time += std::stol(value);
  }
  // total time in clock ticks
  return total_time / sysconf(_SC_CLK_TCK);
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  return getValueOfFile<string>(to_string(pid) + kCmdlineFilename);
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  auto out = findValueByKey<string>("VmRSS:", to_string(pid) + kStatusFilename);
  auto fin = (isNumber(out)) ? std::stoi(out) : -1;
  return std::to_string(fin / 1024);
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  return findValueByKey<string>("Uid:", to_string(pid) + kStatusFilename);
}

// DONE:Read and return the user associated with a process
string LinuxParser::User(int pid) {
  std::ifstream stream(kPasswordPath);
  string line, user, x, uid, id, pwd, e1, e2;
  string find_uid = Uid(pid);

  while (stream.is_open() && std::getline(stream, line)) {
    // make parsing easier
    std::replace(line.begin(), line.end(), ' ', '_');
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream ls(line);

    ls >> user >> x >> uid >> id >> pwd >> e1 >> e2;
    if (uid.compare(find_uid) == 0) {
      break;
    }
  }
  stream.close();

  return user;
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, value;
  vector<string> val;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open() && std::getline(stream, line)) {
    std::istringstream ls(line);
    while (ls >> value) {
      val.emplace_back(value);
    }
  }

  return LinuxParser::UpTime() - (stol(val[21]) / sysconf(_SC_CLK_TCK));
}
