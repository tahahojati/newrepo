#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include "linux_parser.h"
#include "line_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::unordered_map;
using std::unordered_set;

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
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

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
  std::ifstream is(kProcDirectory + kMeminfoFilename);
  LineParser lp;
  const string MemTotal = "MemTotal",
               MemFree = "MemFree";
              //  MemAvailable = "MemAvailable", 
              //  Buffers = "Buffers",
              //  Cached = "Cached",
              //  SReclaimable = "SReclaimable", 
              //  Shmem = "Shmem";
  unordered_map<string, int> map;
  unordered_set<string> wanted_keys{MemTotal, MemFree};
  lp.setModeSeparator(':');
  while (is.is_open() && !wanted_keys.empty()) {
    lp.readLine(is);
    auto ite = wanted_keys.find(lp.getKey());
    if(ite != wanted_keys.end()) {
      wanted_keys.erase(ite);
      map[lp.getKey()] = lp.getValueCount() == 0 ? 0 : std::stoi(lp.getValueAt(0)); //memory in kB.
    }
  }
  if(wanted_keys.empty()){
    //got all the necessary values
    return (map[MemTotal] - map[MemFree]) / (0.0 + map[MemTotal]);
  } 
  return 0.0;
}

long LinuxParser::UpTime() {
  std::ifstream is(kProcDirectory + kUptimeFilename);
  LineParser lp;
  lp.setModeValuesOnly();
  if (is.is_open()) {
    lp.readLine(is);
    return stoi(lp.getValueAt(0));
  }
  return 0;
 }


int _previous_cpu_idle = 0, _previous_cpu_working = 0, _current_cpu_working = 1.0, _current_cpu_idle = 1.0;
float LinuxParser::CpuUtilization() { 
  return (0.0 + _current_cpu_working - _previous_cpu_working) /
    (_current_cpu_working + _current_cpu_idle - _previous_cpu_idle - _previous_cpu_working); 
}

int _totalProcs = 0, _runningProcs = 0;
void LinuxParser::RefreshProcData() {
  std::ifstream is(kProcDirectory + kStatFilename);
  LineParser lp;
  const string Processes = "processes",
               Procs_Running = "procs_running",
               Cpu = "cpu";
  unordered_set<string> wanted_keys{Processes, Procs_Running, Cpu};
  lp.setModeFirstToken();
  while (is.is_open() && !wanted_keys.empty()) {
    lp.readLine(is);
    const string key = lp.getKey();
    auto ite = wanted_keys.find(key);
    if(ite != wanted_keys.end()) {
      wanted_keys.erase(ite);
      if(key == Processes) {
        _totalProcs = lp.getValueCount() == 0 ? 0 : stoi(lp.getValueAt(0));
      } else if (key == Procs_Running) {
        _runningProcs = lp.getValueCount() == 0 ? 0 : stoi(lp.getValueAt(0));
      } else {
        //cpu 234 2342 2342 2342    => sum of first three is working time and the fourth one is idle time.
        _previous_cpu_idle = _current_cpu_idle;
        _previous_cpu_working = _current_cpu_working;
        _current_cpu_working = stoi(lp.getValueAt(0)) + stoi(lp.getValueAt(1)) + stoi(lp.getValueAt(2));
        _current_cpu_idle = stoi(lp.getValueAt(3));
      }
    }
  }
}

int LinuxParser::TotalProcesses() { return _totalProcs; }

int LinuxParser::RunningProcesses() { return _runningProcs; }

string LinuxParser::Command(int pid) {
    std::ifstream is(kProcDirectory + to_string(pid) + kCmdlineFilename);
    string s;
    getline(is, s);
    return s;
}

void LinuxParser::ReadStat(int pid, int& active_time, string& state, int& start_time, unsigned long long& memory, string& name) {
    std::ifstream is(kProcDirectory + to_string(pid) + kStatFilename);
    LineParser lp;
    lp.setModeValuesOnly();
    lp.readLine(is);
    name = lp.getValueAt(1); //name is inside paranthesis so we have to remove the first and last chars.
    name.erase(name.length()-1 ,1);
    name.erase(0, 1);
    state = lp.getValueAt(2);
    active_time = stoi(lp.getValueAt(13)) + stoi(lp.getValueAt(14)); //user time + system time
    start_time = stoi(lp.getValueAt(21));
    memory = std::stoull(lp.getValueAt(22));
}

void LinuxParser::ReadStatus(int pid, int& uid) {
    std::ifstream is(kProcDirectory + to_string(pid) + kStatusFilename);
    LineParser lp;
    lp.setModeSeparator(':');
    const string UID = "Uid";
    while (is.good()) {
        lp.readLine(is);
        if (lp.getKey() == UID) {
            uid = stoi(lp.getValueAt(0));
            return;
        }
    }
    return;
}

unordered_map<int, string> LinuxParser::UidMap() {
    std::ifstream is(kPasswordPath);
    LineParser lp;
    lp.setModeValuesOnly();
    lp.setWordSeparatorToken(':');
    unordered_map<int, string> m;
    while(is.good()) {
        lp.readLine(is);
        if(lp.getValueCount() >= 3) {
            m[stoi(lp.getValueAt(2))] = lp.getValueAt(0);
        }
    }
    return m;
}
