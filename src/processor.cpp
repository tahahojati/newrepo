#include <linux_parser.h>
#include "processor.h"

float Processor::Utilization() const { return LinuxParser::CpuUtilization(); }