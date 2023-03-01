#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
using std::sort;

System::System() {
  cpu_ = Processor{};

  processes_ = {};
  for (int pid : LinuxParser::Pids()) {
    if (LinuxParser::IsRunning(pid)) {
      processes_.emplace_back(Process(pid));
    }
  }
}

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  sort(
      processes_.begin(),
      processes_.end(),
      [] (const Process& a, const Process& b) {
          return b < a;
      }
  );

  return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }
