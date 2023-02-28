#include "format.h"

#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  long minutes = seconds / 60;
  long total_hours = minutes / 60;
  long total_minutes = minutes - (total_hours * 60);
  long total_seconds = seconds - (total_minutes * 60);

  return std::to_string(total_hours) + ":" + std::to_string(total_minutes) +
         ":" + std::to_string(total_seconds);
  /* return Format::Time(seconds, "HH:MM:SS"); */
}