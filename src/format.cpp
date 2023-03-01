#include "format.h"

#include <string>

using std::string;
using std::to_string;

string Format::ElapsedTime(long seconds) {
  long minutes = seconds / 60;

  long total_hours = minutes / 60;
  long total_minutes = minutes - (total_hours * 60);
  long total_seconds = seconds - (total_minutes * 60) - (total_hours * 3600);

  string hours = to_string(total_hours);
  if (hours.length() == 1) {
    hours = "0" + hours;
  }

  string mins = to_string(total_minutes);
  if (mins.length() == 1) {
    mins = "0" + mins;
  }

  string secs = to_string(total_seconds);
  if (secs.length() == 1) {
    secs = "0" + secs;
  }

  return hours + ":" + mins + ":" + secs;
}