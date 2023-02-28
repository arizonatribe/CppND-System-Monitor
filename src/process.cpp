#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::invalid_argument;
using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
  if (pid < 0)
    throw invalid_argument(
        "Process IDs are always non-negative integer values");
  pid_ = pid;
}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() const {
  long total_time = LinuxParser::ActiveJiffies(pid_);
  long total_system_time = LinuxParser::Jiffies();
  return 100.0 * (total_system_time - total_time);
}

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const {
  return Process::CpuUtilization() < a.CpuUtilization();
}