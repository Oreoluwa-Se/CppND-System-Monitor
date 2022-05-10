#include "format.h"

#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  string h = std::to_string(seconds / 3600);
  string s = std::to_string(seconds % 60);
  string m = std::to_string((seconds % 3600) / 60);
  string HH = string(2 - h.length(), '0') + h;
  string SS = string(2 - s.length(), '0') + s;
  string MM = string(2 - m.length(), '0') + m;

  return HH + ":" + MM + ":" + SS;
}