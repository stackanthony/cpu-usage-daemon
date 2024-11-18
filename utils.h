#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>
#include <vector>
pid_t fork_and_kill_parent(void);
void redirect_std_to_dev_null(void);
void setup_working_dir(void);
void setup_pid_file(const std::string &path, const pid_t daemon_pid);
pid_t read_pid_file(const std::string &path);
bool check_pid_health(const pid_t pid);
const char *const bool_to_string(bool b);
std::vector<std::string> split(char delimiter, std::string string);

#ifndef CPU_STRUCT_H
#define CPU_STRUCT_H

struct cpu_usage {
  long user;
  long nice;
  long system;
  long idle;
  long iowait;
  long irq;
  long softirq;
};

#endif

cpu_usage get_cpu_usage();
cpu_usage build_cpu_usage(std::vector<long> &cpu_stats);
double calculate_cpu_percentage(cpu_usage &prev_cpu_usage,
                                cpu_usage &cur_cpu_usage);
