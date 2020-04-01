#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>
#include "process.h"
#include "processor.h"

class System {
 public:
  System();
  const Processor& Cpu() const;
  const std::vector<Process>& Processes() const;
  float MemoryUtilization() const;
  long UpTime() const;
  int TotalProcesses() const;
  int RunningProcesses() const;
  std::string Kernel() const;
  std::string OperatingSystem();
  void RefreshProcData();
  clock_t time() const;
  const std::unordered_map<int, std::string>& UidMap() const;

 private:
  std::string _os = "";
  Processor cpu_ = {};
  std::unordered_map<int, std::string> _uid_map;
  std::vector<Process> processes_ = {};
};

#endif