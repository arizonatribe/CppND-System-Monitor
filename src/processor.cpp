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

  float total_time = working + idle;
  float prev_total_time = prev_working_ + prev_idle_;

  float cumulative_total = total_time - prev_total_time;
  float cumulative_idle = idle - prev_idle_;

  float pct_utilization =
      (cumulative_total - cumulative_idle) / cumulative_total;

  prev_working_ = working;
  prev_idle_ = idle;

  return pct_utilization;
}