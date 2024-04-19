#!/bin/bash

cd ..

# Specify the new directory
new_dir="results"

# Create the new directory if it doesn't exist
mkdir -p "$new_dir"

# Find and copy .dat files
find . -name "*.dat" -exec cp {} "$new_dir" \;

# Find and copy .log files
find . -name "*.log" -exec cp {} "$new_dir" \;
