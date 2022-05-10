#include "system.h"

#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <unistd.h>
#include <vector>

#include "format.h"
#include "linux_parser.h"
#include "process.h"
#include "processor.h"
using std::set;
using std::size_t;
using std::string;
using std::vector;

// DONE: Return the system's CPU
Processor &System::Cpu() { return cpu_; }
vector<Process> &System::Processes() {
  std::vector<Process> build;
  const vector<int> &pids = LinuxParser::Pids();
  for (auto &pid : pids) {
    build.emplace_back(pid);
  }
  processes_.clear();

  for (auto &proc : build) {
    bool check = std::stoi(proc.Ram()) >= 0 && proc.User() != "" &&
                 proc.Command() != "" && proc.UpTime() > 0;
    if (check)
      processes_.emplace_back(proc);
  }
  std::sort(processes_.begin(), processes_.end());

  return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }
long System::UpTime() { return LinuxParser::UpTime(); }
