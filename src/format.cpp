#include <string>
#include <sstream>
#include <iomanip>
#include "format.h"

using std::string;
using std::setw;
using std::setfill;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int s, m, h;
    std::ostringstream os;
    s = seconds % 60;
    m = (seconds /= 60) % 60;
    h = seconds / 60; 
    os << setw(2) << setfill('0') << h << ":";
    os << setw(2) << setfill('0') << m << ":";
    os << setw(2) << setfill('0') << s << ":";
    return os.str(); 
}