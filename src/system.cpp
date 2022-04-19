#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "format.h"
#include "linux_parser.h"
#include "process.h"
#include "processor.h"
using std::set;
using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric
criteria "The student will be able to extract and display basic data about the
system."

You need to properly format the uptime. Refer to the comments mentioned in
format. cpp for formatting the uptime.*/

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }
vector<Process>& System::Processes() {
    processes_.clear();
    vector<int> pids = LinuxParser::Pids();
    processes_.reserve(pids.size());

    for (auto pid : pids){
        Process process(pid);
        processes_.emplace_back(process);
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
