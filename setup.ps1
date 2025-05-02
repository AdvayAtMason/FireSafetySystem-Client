# Windows PowerShell Script - Windows 10 and later
# Script to set up a Python virtual environment and install dependencies

# Check if Python is installed
if (-not (Get-Command python3 -ErrorAction SilentlyContinue)) {
    Write-Host "Python is not installed. Please install Python 3.x and try again."
    exit 1
}

# Create a virtual environment
python3 -m venv venv

# Check if the virtual environment was created successfully
if (-not (Test-Path -Path "venv" -PathType Container)) {
    Write-Host "Failed to create virtual environment."
    exit 1
}

# Activate the virtual environment
. venv\Scripts\Activate.ps1

# Install the required dependencies
pip install -r requirements.txt

Write-Host "Setup complete! Virtual environment created and dependencies installed."