# Linux Bash Script - Debian/Ubuntu
# Script to set up a Python virtual environment and install dependencies

#!/bin/bash

# Check if Python3 is installed
if ! command -v python3 &> /dev/null; then
    echo "Python3 is not installed. Please install Python3 and try again."
    exit 1
fi

# Create a virtual environment
python3 -m venv venv

# Check if the virtual environment was created successfully
if [ ! -d "venv" ]; then
    echo "Failed to create virtual environment."
    exit 1
fi

# Gives execution permission to the environment activation script
chmod +x venv/bin/activate

# Activate the virtual environment
source venv/bin/activate

# Install the required dependencies
pip install -r requirements.txt

echo "Setup complete! Virtual environment created and dependencies installed."