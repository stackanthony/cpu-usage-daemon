#include "utils.h"

pid_t fork_and_kill_parent() {
  pid_t child_pid = fork(); // 0x1 = 10002

  if (child_pid == -1) {
    throw std::runtime_error(&"Unable to create child process."[errno]);
  }

  if (child_pid != 0) {
    // If pid isn't 0 or -1, this is the parent process, we have to kill it.
    exit(EXIT_SUCCESS);
  }

  return child_pid;
}

void setup_working_dir(void) {
  int status_code = chdir("/");
  if (status_code != 0) {
    throw std::runtime_error("Unable to change directory to /: " +
                             std::string(strerror(errno)));
  }

  umask(0);

  if (close(STDIN_FILENO) == -1) {
    syslog(LOG_ERR, "Failed to close STDIN: %s", strerror(errno));
  }

  if (close(STDOUT_FILENO) == -1) {
    syslog(LOG_ERR, "Failed to close STDOUT: %s", strerror(errno));
  }

  if (close(STDERR_FILENO) == -1) {
    syslog(LOG_ERR, "Failed to close STDERR: %s", strerror(errno));
  }

  syslog(LOG_INFO, "Set Up Working Directory!");
}

void setup_pid_file(const std::string &path, const pid_t daemon_pid) {
  std::ofstream pid_file(path, std::ios::out | std::ios::trunc);

  if (!pid_file.is_open()) {
    syslog(LOG_ERR, "Failed to open PID file %s: %s", path.c_str(),
           strerror(errno));
    return;
  }

  pid_file << daemon_pid << std::endl;

  if (pid_file.fail()) {
    syslog(LOG_ERR, "Failed to write to PID file %s", path.c_str());
    return;
  }

  syslog(LOG_INFO, "Successfully wrote PID %d to file %s", daemon_pid,
         path.c_str());
}

pid_t read_pid_file(const std::string &path) {
  std::fstream fs;
  fs.open(path);

  if (!fs.is_open()) {
    syslog(LOG_ERR, "Unable to read PID file: %s", path.c_str());
    return -1;
  }

  std::string pid_str;
  std::getline(fs, pid_str);
  pid_t pid = std::stoi(pid_str);

  return pid;
}

bool check_pid_health(const pid_t pid) { return (getpgid(pid) >= 0); }

std::vector<std::string> split(char delimiter, std::string string) {

  std::vector<std::string> split_strings;

  std::string split_word = "";
  for (int i = 0; i < string.size(); i++) {
    if (string[i] == delimiter) {
      if (split_word != "") {
        split_strings.push_back(split_word);
      }
      split_word = "";
      continue;
    }

    split_word += string[i];
  }

  split_strings.push_back(split_word);
  return split_strings;
}

cpu_usage build_cpu_usage(std::vector<long> &cpu_stats) {
  cpu_usage cpu_usage;

  cpu_usage.user = cpu_stats[0];
  cpu_usage.nice = cpu_stats[1];
  cpu_usage.system = cpu_stats[2];
  cpu_usage.idle = cpu_stats[3];
  cpu_usage.iowait = cpu_stats[4];
  cpu_usage.irq = cpu_stats[5];
  cpu_usage.softirq = cpu_stats[6];

  return cpu_usage;
}

cpu_usage get_cpu_usage() {
  std::fstream fs;

  fs.open("/proc/stat");

  if (!fs.is_open()) {
    syslog(LOG_ERR, "Unable to read /proc/stat file: %s", strerror(errno));
    throw std::runtime_error("Unable to read /proc/stat");
  }

  std::string cpu_usage_line;
  std::getline(fs, cpu_usage_line);

  std::vector<std::string> split_strings = split(' ', cpu_usage_line);
  split_strings.erase(
      split_strings.begin()); // Removes "cpu" from the split_strings

  // Builds the cpu stats vector which we'll use to build our struct
  std::vector<long> cpu_stats;
  for (const std::string cpu_stat : split_strings) {
    cpu_stats.push_back(std::stoi(cpu_stat));
  }

  return build_cpu_usage(cpu_stats);
}

double calculate_cpu_percentage(cpu_usage &prev_cpu_usage,
                                cpu_usage &cur_cpu_usage) {

  long user_diff = cur_cpu_usage.user - prev_cpu_usage.user;
  long nice_diff = cur_cpu_usage.nice - prev_cpu_usage.nice;
  long system_diff = cur_cpu_usage.system - prev_cpu_usage.system;
  long idle_diff = cur_cpu_usage.idle - prev_cpu_usage.idle;
  long iowait_diff = cur_cpu_usage.iowait - prev_cpu_usage.iowait;
  long irq_diff = cur_cpu_usage.irq - prev_cpu_usage.irq;
  long softirq_diff = cur_cpu_usage.softirq - prev_cpu_usage.softirq;

  long total_diff = user_diff + nice_diff + system_diff + idle_diff +
                    iowait_diff + irq_diff + softirq_diff;

  long active_diff =
      user_diff + nice_diff + system_diff + irq_diff + softirq_diff;

  // Guardrail for divide by 0 exception
  if (total_diff == 0)
    return 0.0;

  return (active_diff * 100.0) / total_diff;
}
