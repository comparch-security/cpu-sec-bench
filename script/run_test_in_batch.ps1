param(
    [Parameter(Mandatory=$true)]
    [string]$csv_file
)

$collect_dir = "results"

Set-Location ..
$currentLocation = Get-Location
Remove-Item -Path $collect_dir -Recurse -Force
New-Item -Path $collect_dir -ItemType Directory
Set-Location "cpu-sec-bench" -ErrorAction Stop

$one_run_ind=0
Get-Content $csv_file | ForEach-Object {
    $parts = $_ -split ','
    $prefix = $parts[0]
    $vars = $parts[1..($parts.Length - 1)]

    Write-Output "prefix: $prefix"

    # Write-Output "rest_columns: $rest_columns"

    $null=Start-Job -ScriptBlock{
        Set-Location $using:currentLocation -ErrorAction Stop
        if ($using:one_run_ind -ne 0) {
            $new_dir = "cpu-sec-bench-$using:one_run_ind"
            Remove-Item -Path $new_dir -Recurse -Force
            Copy-Item -Path "cpu-sec-bench" -Destination $new_dir -Recurse
            Set-Location $new_dir -ErrorAction Stop
        }else{
            Set-Location "cpu-sec-bench" -ErrorAction Stop
        }
        Import-Module .\script\start-job-function.ps1

        foreach ($var in $using:vars) {
            Write-Output "var: $var"
            $part = $var -split '='
            $key = $part[0]
            $value = $part[1]
            Set-Item -Path Env:\$key -Value "$value"
        }

        run_test "$using:prefix" "$using:collect_dir"
    }

    $one_run_ind++
}

# Get all jobs
$jobs = Get-Job

# Wait for all jobs to complete
$jobs | Wait-Job

# Retrieve and print the results of each job
foreach ($job in $jobs) {
    $result = Receive-Job -Job $job
    Write-Output "Result of job $($job.Id): $result"
}

# Clean up the jobs
$jobs | Remove-Job