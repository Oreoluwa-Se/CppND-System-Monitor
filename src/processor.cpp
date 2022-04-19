#include "processor.h"
#include "linux_parser.h"
#include <iostream>

// Done: Return the aggregate CPU utilization
float Processor::Utilization() { 
    long curr_active = LinuxParser::ActiveJiffies();
    long curr_idle = LinuxParser::IdleJiffies();

    long dur_active = curr_active - prev_active_;
    long dur_idle = curr_idle - prev_idle_;

    auto val = (float)(dur_active / (dur_active + dur_idle));

    prev_idle_ = curr_idle;
    prev_active_ = curr_active;
    
    return val;
}