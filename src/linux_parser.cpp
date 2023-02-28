#include "linux_parser.h"

#include <dirent.h>
#include <math.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>

using LinuxParser::CPUStates;
using std::stof;
using std::stoi;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  long total_memory, free_memory;

  string line;
  string key;
  string value;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          total_memory = stol(value);
        } else if (key == "MemFree") {
          free_memory = stol(value);
        }
      }
    }
  }

  return total_memory - free_memory;
}

float LinuxParser::CachedMemoryUtilization() {
  long cached, sreclaimable, shmem;

  string line;
  string key;
  string value;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Cached") {
          cached = stol(value);
        } else if (key == "SReclaimable") {
          sreclaimable = stol(value);
        } else if (key == "Shmem") {
          shmem = stol(value);
        }
      }
    }
  }

  return cached + sreclaimable - shmem;
}

float LinuxParser::BufferMemoryUtilization() {
  long buffers;

  string line;
  string key;
  string value;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Buffers") {
          buffers = stol(value);
        }
      }
    }
  }

  return buffers;
}

long LinuxParser::UpTime() {
  long uptime;
  string line;
  string str_uptime;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> str_uptime;
    uptime = stol(str_uptime);
  }
  return uptime;
}

long LinuxParser::IdleTime() {
  long idletime;
  string line;
  string _uptime, str_idletime;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> _uptime >> str_idletime;
    idletime = stol(str_idletime);
  }
  return idletime;
}

long LinuxParser::Jiffies() {
  long uptime = LinuxParser::UpTime();
  return uptime * sysconf(_SC_CLK_TCK);
}

long LinuxParser::ActiveJiffies(int pid) {
  long total_time;
  string line;
  string _pid, _comm, _state, _ppid, _pgrp, _session, _tty_nr, _tpgid, _flags,
      _minflt, _cminflt, _majflt, _cmajflt, utime, stime, cutime, cstime;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> _pid >> _comm >> _state >> _ppid >> _pgrp >> _session >>
        _tty_nr >> _tpgid >> _flags >> _minflt >> _cminflt >> _majflt >>
        _cmajflt >> utime >> stime >> cutime >> cstime;
    total_time = stol(utime) + stol(stime) + stol(cutime) + stol(cstime);
  }

  return total_time;
}

long LinuxParser::ActiveJiffies() {
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

long LinuxParser::IdleJiffies() {
  long idletime = LinuxParser::IdleTime();
  return idletime * sysconf(_SC_CLK_TCK);
}

vector<string> LinuxParser::CpuUtilization() {
  string line;
  long working_time = 0;
  long non_working_time = 0;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.rfind("cpu", 0)) {
        line.replace(0, 3, "");
        break;
      }
    }

    std::istringstream linestream(line);

    string stat;
    int column = 0;

    while (linestream >> stat) {
      switch (column++) {
        case kUser_:
        case kNice_:
        case kSystem_:
        case kIRQ_:
        case kSoftIRQ_:
        case kSteal_: {
          working_time += stol(stat);
          break;
        }
        case kIdle_:
        case kIOwait_: {
          non_working_time += stol(stat);
          break;
        }
        // guest and guest_nice are already factored into the user and nice
        // stats
        default:
          break;
      }
    }
  }

  return {std::to_string(working_time), std::to_string(non_working_time)};
}

int LinuxParser::TotalProcesses() {
  string line;
  string key, value;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return stoi(value);
        }
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  string line;
  string key, value;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return stoi(value);
        }
      }
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  string _pid, comm;
  string line;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> comm;
  }

  if (comm.empty()) {
    std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
    if (stream.is_open()) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      string _pid, comm;
      linestream >> _pid >> comm;
    }
  }

  return comm;
}

// Using statm since a bunch of processes didn't have Vm* fields in their
// `/proc/<PID>/status` files (at least on my Fedora machine)
string LinuxParser::Ram(int pid) {
  string memory;
  string line;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatMFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> memory;
  }

  return std::to_string(floor(stof(memory) / 1000)) + " MB";
}

string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), ' ', '-');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::User(int pid) {
  string username, _pwd_flag, uid, _gid, name;
  string line;

  string real_uid = LinuxParser::Uid(pid);

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> username >> _pwd_flag >> uid >> _gid >> name) {
        if (uid == real_uid) {
          std::replace(name.begin(), name.end(), '_', ' ');
          return !name.empty() ? username + " (" + name + ")" : username;
        }
      }
    }
  }

  return username;
}

long LinuxParser::UpTime(int pid) {
  long proc_uptime;
  string line;
  string _pid, _comm, _state, _ppid, _pgrp, _session, _tty_nr, _tpgid, _flags,
      _minflt, _cminflt, _majflt, _cmajflt, _utime, _stime, _cutime, _cstime,
      _priority, _nice, _num_threads, _itrealvalue, starttime;

  long system_uptime = LinuxParser::UpTime();

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> _pid >> _comm >> _state >> _ppid >> _pgrp >> _session >>
        _tty_nr >> _tpgid >> _flags >> _minflt >> _cminflt >> _majflt >>
        _cmajflt >> _utime >> _stime >> _cutime >> _cstime >> _priority >>
        _nice >> _num_threads >> _itrealvalue >> starttime;
    proc_uptime = stol(starttime);
  }

  return system_uptime - (proc_uptime / sysconf(_SC_CLK_TCK));
}
