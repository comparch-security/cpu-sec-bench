# Change to the parent directory
Set-Location ..

# Specify the new directory
$new_dir = "results"

# Create the new directory if it doesn't exist
if(!(Test-Path -Path $new_dir)) {
    New-Item -ItemType Directory -Path $new_dir
}

# Find and copy .dat files
Get-ChildItem -Recurse -Filter "*.dat" | ForEach-Object {
    Copy-Item -Path $_.FullName -Destination $new_dir
}

# Find and copy .log files
Get-ChildItem -Recurse -Filter "*.log" | ForEach-Object {
    Copy-Item -Path $_.FullName -Destination $new_dir
}

Set-Location "cpu-sec-bench"