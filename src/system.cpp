#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <linux_parser.h>
#include <ctime>

#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

const Processor& System::Cpu() const { return cpu_; }

const vector<Process>& System::Processes() const { return processes_; }

std::string System::Kernel() const { return LinuxParser::Kernel(); }

float System::MemoryUtilization() const { return LinuxParser::MemoryUtilization(); }

clock_t System::time() const {
    return clock();
}

std::string System::OperatingSystem() {
    if(_os == "") {
        _os = LinuxParser::OperatingSystem();
    }
    return _os;
}

void System::RefreshProcData() {
    LinuxParser::RefreshProcData();
    vector<int> pids = LinuxParser::Pids();
    processes_.clear();
    for(int i: pids) {
        Process p(i, this);
        try {
        p.ReadInfo();

        } catch(...) {
            //do nothing
        }
        processes_.push_back(p);
    }
}

int System::RunningProcesses() const{ return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() const{ return LinuxParser::TotalProcesses(); }

const std::unordered_map<int, string>& System::UidMap() const {
    return _uid_map;
}

System::System() {
    _uid_map = LinuxParser::UidMap();
}

long int System::UpTime() const { return LinuxParser::UpTime(); }