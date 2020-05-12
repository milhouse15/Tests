#include <string>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    long int hours, minutes, ss, mm;

    ss = seconds%60;
    minutes = seconds/60;
    mm = minutes%60;
    hours = minutes/60;
  
  	std::string time = std::to_string(hours) + ":" + std::to_string(mm) + ":" + std::to_string(ss);
    
  return time; 
}
