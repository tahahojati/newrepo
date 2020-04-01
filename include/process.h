#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class System;
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  inline Process(int pid, System* system): _pid(pid), _system(system){}
  void ReadInfo();
  int Pid() const;
  std::string User() const;
  std::string Command() const;
  float CpuUtilization() const;
  std::string Ram() const;
  long int UpTime() const;
  bool operator<(Process const& a) const;

 private:
  const int _pid;
  const System* _system;
  int active_time=1, start_time=0, uid, previous_active_time, uptime=1, previous_uptime;
  unsigned long long memory; 
  std::string cmd, state, name, user_name;
};

#endif