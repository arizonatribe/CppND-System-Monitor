#include "processor.h"

#include <string>
#include <vector>

#include "linux_parser.h"
using std::stof;
using std::string;

float Processor::Utilization() {
  std::vector<string> stats = LinuxParser::CpuUtilization();

  float working = stof(stats[0]);
  float idle = stof(stats[1]);

  float total_working_time = working - prev_working_;
  float total_idle_time = idle - prev_idle_;

  float pct_utilization =
      (total_working_time - total_idle_time) / total_working_time;

  prev_working_ = working;
  prev_idle_ = idle;

  return pct_utilization;
}