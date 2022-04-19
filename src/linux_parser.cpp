#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
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
  float total{-1}, free{-1};
  string line, item, value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);

  while (stream.is_open() && std::getline(stream, line)) {
    std::istringstream ls(line);
    ls >> item >> value;
    if (item.find("MemTotal") != std::string::npos) {
      total = std::stof(value);
    } else if (item.find("MemFree") != std::string::npos) {
      free = std::stof(value);
    }

    // terminating condition
    if (total > 0 && free > 0) break;
  }

  return (total - free) / total;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string line, value;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open() && std::getline(stream, line)) {
    std::istringstream ls(line);
    ls >> value;
  }

  return std::stol(value);
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> utilized;
  string line;
  string value;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream ls(line);
    ls >> value; 
    while (ls >> value) {
      if (value == "cpu") ls >> value; 
      utilized.emplace_back(value);
    }
  }
  return utilized;
}

// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long total{0};
  vector<string> jiffs = CpuUtilization();
  for (auto& jval : jiffs) {
    total += std::stol(jval);
  }
  return total;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> jiff = CpuUtilization();
  return std::stol(jiff[CPUStates::kIdle_]) +
         std::stol(jiff[CPUStates::kIOwait_]);
}

// DONE: Read and return the number of Active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return Jiffies() - IdleJiffies();
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string val;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  while (stream.is_open() && std::getline(stream, line)) {
    std::istringstream ls(line);
    ls >> val;

    if (val.compare("processes") == 0) {
      ls >> val;
      return std::stoi(val);
    }
  }
  return 0;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string val;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  while (stream.is_open() && std::getline(stream, line)) {
    std::istringstream ls(line);
    ls >> val;

    if (val.compare("procs_running") == 0) {
      ls >> val;
      return std::stoi(val);
    }
  }
  return 0;
}

// DONE: Read and return the number of active jiffies for a PID
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
long LinuxParser::ActiveJiffies(int pid) {
  long total_time{0};
  int idx{0};
  string line, value;
  int key_loc[] = {14, 15, 16, 17};
  
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                       LinuxParser::kStatFilename);
  if (stream.is_open() && std::getline(stream, line)){
      std::istringstream ls(line);
      while (ls >> value && idx < key_loc[3]) {
        if (idx >= key_loc[0]) {
          total_time += std::stol(value);
        }
        idx += 1;
      }
      ls >> value;
      total_time += std::stol(value);
  }
  // total time in clock ticks
  return total_time / sysconf(_SC_CLK_TCK);
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string value, line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  while (stream.is_open() && std::getline(stream, line)) {
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream ls(line);
    ls >> value;
    if (value.compare("VmSize") == 0) {
        ls >> value;
        break;
        }
      }
  return std::to_string(std::stoi(value) / 1024);
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string key, value, line;

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  while(stream.is_open() && std::getline(stream, line)){
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream ls(line);    
    while (ls >> key >> value){
      if (key.compare("Uid") == 0){
        break;
      }
    }
  }
  return value;
 }

// DONE:Read and return the user associated with a process
string LinuxParser::User(int pid) {
  std::ifstream stream(kPasswordPath);
  string line, user, x, uid, id, pwd, e1, e2;
  string find_uid = Uid(pid);

  while(stream.is_open() && std::getline(stream, line)){
    // make parsing easier  
    std::replace(line.begin(), line.end(), ' ', '_');
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream ls(line);
    
    ls >> user >> x >> uid >> id >> pwd >>e1 >> e2;
    if (uid.compare(find_uid) == 0){
      break;
    }
  }

  return user;
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  int idx{0};
  string line, value;
  int key_loc[] = {21};

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open() && std::getline(stream, line)) {
    std::istringstream ls(line);
    while (ls >> value) {
      if (idx == key_loc[0]) {
        break;
      }
      idx += 1;
    }
  }

  return std::stol(value) / sysconf(_SC_CLK_TCK);
}

long LinuxParser::StartTime(int pid) {
  int idx{0};
  string line, value;
  int key_loc[] = {22};

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open() && std::getline(stream, line)) {
    std::istringstream ls(line);
    while (ls >> value) {
      if (idx == key_loc[0]) {
        break;
      }
      idx += 1;
    }
  }

  return std::stol(value) / sysconf(_SC_CLK_TCK);
}