#ifndef PROCESS_H
#define PROCESS_H

#include <string>

#include "linux_parser.h"
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
public:
  explicit Process(int p);
  int Pid();                              // DONE: See src/process.cpp
  std::string User();                     // DONE: See src/process.cpp
  std::string Command();                  // DONE: See src/process.cpp
  float CpuUtilization();                 // DONE: See src/process.cpp
  std::string Ram();                      // DONE: See src/process.cpp
  long int UpTime();                      // DONE: See src/process.cpp
  bool operator<(const Process &a) const; // DONE: See src/process.cpp

  // TODO: Declare any necessary private members
private:
  int pid_{};
  std::string ram_{};
};

#endif