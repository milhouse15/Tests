#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
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

// DONE----------------: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line, key, value;
  float total, free, buffers;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          total = stof(value);
        }
       	if (key == "MemFree:") {
          free = stof(value);
        }
        if (key == "Buffers:") {
          buffers = stof(value);
        }
      }
    }
  }
	return 1.0-(free/(total-buffers));
}

// DONE----------------: Read and return the system uptime
long LinuxParser::UpTime() {  
  string uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return stol(uptime);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// DONE----------------: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization(){
  vector<string> data;
  string line, key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i <= 10; i++) {
    	linestream >> value;
      	data.push_back(value);
    }
  }
  data.erase(data.begin());
  return data;
}
// DONE----------------: Read and return CPU utilization
float LinuxParser::CpuUtilization(int pid){
  string line;
  string value;
  vector<int> keys {14, 15, 16, 17, 22};
  vector<int> values;
  long upTime = UpTime();
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);   
      int c = 0;
      while (c!=22)
      {
        linestream>>value;
        if (std::find(keys.begin(), keys.end(), c+1) != keys.end()) {
        	values.push_back(stoi(value));
        }
      	c++;
      }
    }
  }
  float totalTime = values[0]+values[1]+values[2]+values[3];
  float seconds = upTime - (values[5]/sysconf(_SC_CLK_TCK));
  float cpuUtilization = (totalTime/sysconf(_SC_CLK_TCK))/seconds;
  return cpuUtilization;
}
// DONE----------------: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  string value;
  string total;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") 
        {
          total = value;
        }
      }
    }
  }
  return stoi(total); 
}

// DONE----------------: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  string value;
  string running;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") 
        {
          running = value;
        }
      }
    }
  }
  return stoi(running); 
  }

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) { std::getline(stream, line); }
  return line;
}
// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {  
  string line;
  string valueString;
  float value = 0;
  int c = 0;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);  
      if (c==17) {
        linestream >> valueString >> valueString;
        value = stof(valueString);
      }
      c++;
    }
  }
  string ramUtilization = to_string(value/1000);
  string returnData;
  for (int i = 0; i < ramUtilization.size()-1; i++) {
    returnData += ramUtilization[i];
    if (ramUtilization[i-1] == '.') { break; }
  }
  return returnData;
} 


// DONE----------------: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {   
  string uid, key, value;
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
    if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") 
        {
          uid = value;
        }
      }
    }
  }
  return uid; 
}
  
// DONE----------------: Read and return the user associated with a process
string LinuxParser::User(int pid) {   
  string uid, key, value, value2, line;
  uid = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value >> value2) {
        if (value2 == uid) 
        {
          return key;
        }
      }
    }
  }
}

// DONE----------------: Read and return the uptime of a process
long int LinuxParser::UpTime(int pid) { 
  string upTime;
  string line;
  std::ifstream stream(kProcDirectory +to_string(pid)+ kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    int x = 0;
    while (x != 22)
    {
      linestream >> upTime;
      x++;
    }
  }
  return stol(upTime)/sysconf(_SC_CLK_TCK); 
}
