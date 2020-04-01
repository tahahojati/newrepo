#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <linux_parser.h>
#include "process.h"
#include "system.h"

using std::string;
using std::to_string;
using std::vector;

void Process::ReadInfo(){
    previous_active_time = active_time;
    previous_uptime = uptime;
    LinuxParser::ReadStat(_pid, active_time, state, start_time, memory, name);
    LinuxParser::ReadStatus(_pid, uid);
    cmd = LinuxParser::Command(_pid);
    uptime = (int)(_system -> time() - start_time);
    std::unordered_map<int, std::string> uid_map = _system -> UidMap();
    try {
        user_name = uid_map.at(uid);
    } catch (...) {
        //do nothing
    }
}

int Process::Pid() const { return _pid; }

float Process::CpuUtilization() const {
    return (0.0 + active_time - previous_active_time)/(uptime - previous_uptime);
}

string Process::Command() const { return cmd; }

string Process::Ram() const { return std::to_string((memory + 0.0) / 1e6) + " MB"; }

string Process::User() const { return user_name; }

long int Process::UpTime() const { return uptime; }

bool Process::operator<(Process const& a) const { return CpuUtilization() < a.CpuUtilization(); }
