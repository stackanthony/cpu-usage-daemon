#!/bin/zsh

wait_for_file() {
    local file=$1
    local timeout=${2:-60}
    local start_time=$(date +%s)
    
    while [ ! -f "$file" ]; do
        if [ $(($(date +%s) - start_time)) -ge $timeout ]; then
            echo "Timeout waiting for $file"
            exit 1
        fi
        sleep 1
    done
    
    # Add a small delay to ensure file is fully written
    sleep 1
}

cleanup() {
    if [ -f "$path_config" ]; then
      echo -e "\nStopping daemon..."
    fi

    if [ -f "$PID_FILE_PATH" ]; then
        DAEMON_PID=$(cat "$PID_FILE_PATH")
        sudo kill -TERM $DAEMON_PID
        rm -f "$PID_FILE_PATH"  
    fi
    rm -f "path_config.txt"
    exit 0
}

trap cleanup SIGINT

# Kill any existing tail processes at startup
pkill -f "tail -f /var/log/syslog"

# Compile and run the daemon
g++ -o ./mydaemon main.cpp utils.cpp
sudo ./mydaemon

local path_config="path_config.txt"
# Wait for the configuration file
wait_for_file $path_config

# Read the path from the configuration file
PID_FILE_PATH=$(cat $path_config)
if [ -z "$PID_FILE_PATH" ]; then
    echo "Error: Path configuration is empty"
    exit 1
fi

echo "Using PID file path: $PID_FILE_PATH"

# Start new tail process
tail -f /var/log/syslog | grep --line-buffered mydaemon
