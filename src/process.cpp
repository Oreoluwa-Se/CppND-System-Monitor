#include "process.h"
#include <iostream>
#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// constructor
Process::Process(int pid) : pid_(pid) { ram_ = Process::Ram(); }

// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
float Process::CpuUtilization() {
  float val = 0.0;
  auto uptime = (float)Process::UpTime();
  if (uptime > 0)
    return (float)LinuxParser::ActiveJiffies(pid_) / uptime;

  return val;
}

// DONE: Return the command that generated this process
string Process::Command() {
  auto ret_val = LinuxParser::Command(pid_);
  if (ret_val.length() > 40) {
    return ret_val.substr(0, 40);
  }
  return ret_val;
}

// DONE: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(const Process &a) const {
  return std::stoi(this->ram_) > std::stoi(a.ram_);
}
