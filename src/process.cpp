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
Process::Process(int pid):pid_(pid){
  ram_ = LinuxParser::Ram(Pid()); 
  utilize_ = CpuUtilization();
  }

// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
float Process::CpuUtilization() {
  long c_active = LinuxParser::ActiveJiffies(pid_);
  long c_total = LinuxParser::UpTime() - LinuxParser::StartTime(pid_);
  auto out = 100 * c_active / c_total;
  return (float) out;
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// DONE: Return this process's memory utilization
string Process::Ram() { return ram_; }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return (long int)LinuxParser::UpTime(Pid()); }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(const Process& a) const {
  return std::stof(ram_) < std::stof(a.ram_);
}
