# CPU Usage Daemon
![](https://i.imgur.com/oDb0t4H.gif)
<!--toc:start-->

- [CPU Monitoring Daemon](#cpu-monitoring-daemon)
  - [Features](#features)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Usage](#usage)
  - [Configuration](#configuration)
  - [Technical Details](#technical-details)
    - [Process Management](#process-management)
    - [CPU Monitoring](#cpu-monitoring)
    - [Error Handling](#error-handling)
  - [Logging](#logging)
  - [Error Codes](#error-codes)
  - [Contributing](#contributing)
  - [License](#license)
  - [Acknowledgments](#acknowledgments)
  <!--toc:end-->

A lightweight system daemon that monitors CPU usage in real-time and logs the metrics to syslog. This daemon is designed to run in the background and provide continuous CPU utilization statistics with minimal system overhead.

## Features

- Runs as a proper Unix daemon process
- Real-time CPU usage monitoring
- Secure process isolation with double-forking
- Configurable PID file location
- Detailed syslog integration
- Clean process management and termination
- Automated build and run script

## Prerequisites

- Linux-based operating system
- G++ compiler
- Root privileges (for daemon operations)
- System logger (syslog) configured

## Installation

1. Clone the repository:

```bash
git clone https://github.com/stackanthony/cpu-usage-daemon.git
cd cpu-usage-daemon
```

2. Compile the project:

```bash
g++ -o mydaemon main.cpp utils.cpp
```

## Usage

The project includes a convenient shell script to handle the building, running, and monitoring of the daemon.

1. Run the daemon:

```bash
./launch.sh
```

2. The script will:

   - Compile the source code
   - Prompt for a PID file location
   - Launch the daemon process
   - Start monitoring the syslog output

3. Monitor the daemon's output:

   - CPU usage statistics will be logged to syslog
   - The script automatically filters and displays relevant log entries

4. To stop the daemon:
   - Press `Ctrl+C` in the terminal running the script
   - The cleanup process will automatically terminate the daemon and remove the PID file

## Configuration

The daemon supports the following configurations:

- **PID File Location**: Specified at runtime
- **Polling Rate**: Default is 1 second (can be modified in `main.cpp`)
- **Working Directory**: Automatically set to root (/)
- **Log Level**: Uses syslog with INFO level for metrics and ERR level for errors

## Technical Details

### Process Management

- Implements the double-fork technique for proper daemonization
- Creates a new session using `setsid()`
- Manages standard file descriptors
- Handles process isolation and zombie prevention

### CPU Monitoring

- Reads CPU statistics from `/proc/stat`
- Calculates CPU usage percentage across all cores
- Tracks user, system, idle, I/O wait, and interrupt time
- Updates metrics at configurable intervals

### Error Handling

- Comprehensive error checking and reporting
- Secure file operations
- Proper resource cleanup on termination
- Detailed error logging through syslog

## Logging

The daemon logs its activity to syslog with the following format:

- CPU usage metrics: `LOG_INFO` level
- Operational messages: `LOG_NOTICE` level
- Error conditions: `LOG_ERR` level

Monitor the logs in real-time:

```bash
tail -f /var/log/syslog | grep --line-buffered mydaemon
```

## Error Codes

The daemon may exit with the following status codes:

- 0: Normal termination
- 1: Configuration error
- Other non-zero values: System-specific error codes

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

[MIT License]

## Acknowledgments

- Based on standard Unix daemon design patterns
- Utilizes Linux process management best practices
