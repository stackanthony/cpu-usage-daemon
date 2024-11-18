#include "main.h"
#include <ostream>
int polling_rate_seconds = 1;

int main(void) {
  std::string path;

  std::cout << "Enter path for PID file (Will create if path doesn't exist): ";
  std::cin >> path;
  std::cout << std::endl;

  std::ofstream pathFile("path_config.txt");
  if (pathFile.is_open()) {
    pathFile << path;
    pathFile.close();
  } else {
    std::cerr << "Error: Could not write path configuration" << std::endl;
    return 1;
  }

  fork_and_kill_parent();
  pid_t pgid_of_child = setsid();

  if (pgid_of_child == -1) {
    throw std::runtime_error(
        &"Unable to create new session for child process: "[errno]);
  }

  fork_and_kill_parent();

  pid_t daemon_pid = getpid();

  setup_working_dir();

  setup_pid_file(path, daemon_pid);

  if (check_pid_health(daemon_pid)) {
    while (true) {
      cpu_usage prev_cpu_usage = get_cpu_usage();
      sleep(1);
      cpu_usage cur_cpu_usage = get_cpu_usage();

      double cpu_percentage =
          calculate_cpu_percentage(prev_cpu_usage, cur_cpu_usage);

      syslog(LOG_INFO, "CPU Usage: %f%%", cpu_percentage);
      sleep(polling_rate_seconds);
    }
  }

  syslog(LOG_NOTICE, "Daemon terminated normally");

  return 0;
}
