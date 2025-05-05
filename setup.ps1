# NOTE: This script will be moved to the scripts directory in the future.
# Windows PowerShell Script - Windows 10 and later
# Script to set up a Python virtual environment and install dependencies

# Check if Python is installed
if (-not (Get-Command python3 -ErrorAction SilentlyContinue)) {
    Write-Host "Python is not installed. Please install Python 3.x and try again."
    exit 1
}

# Check if pip is installed
if (-not (Get-Command pip -ErrorAction SilentlyContinue)) {
    Write-Host "pip is not installed. Please install pip and try again."
    exit 1
}

# Check if requirements.txt exists
if (-not (Test-Path -Path "requirements.txt")) {
    Write-Host "requirements.txt file not found. Please ensure it exists in the current directory."
    exit 1
}

# Check if virtualenv is installed
if (-not (Get-Command virtualenv -ErrorAction SilentlyContinue)) {
    Write-Host "virtualenv is not installed. Please install it using 'pip install virtualenv' and try again."
    exit 1
}

# Create a virtual environment
if (Test-Path -Path "venv") {
    Write-Host "Virtual environment already exists. Skipping creation."
} else {
    Write-Host "Creating virtual environment..."
    python3 -m venv venv
}


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