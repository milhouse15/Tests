#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
	vector<string> CPU = LinuxParser::CpuUtilization();
  	float Idle, NonIdle, Total, totald, idled;
  
  	Idle = stof(CPU[LinuxParser::kIdle_]) + stof(CPU[LinuxParser::kIOwait_]);
  	NonIdle = stof(CPU[LinuxParser::kUser_]) + stof(CPU[LinuxParser::kNice_]) + stof(CPU[LinuxParser::kSystem_]) + stof(CPU[LinuxParser::kIRQ_]) + stof(CPU[LinuxParser::kSoftIRQ_]) + stof(CPU[LinuxParser::kSteal_]);
    Total = Idle + NonIdle;
  	totald = Total - prevTotal; 
  	idled = Idle - prevIdle;
  	prevTotal = Total;
  	prevIdle = Idle;
  	prevNonIdle = NonIdle;
  	return (totald-idled)/totald;
  
}
