function rename_log {
    $log_name = Get-ChildItem -Filter *.dat | Sort-Object LastAccessTime -Descending | Select-Object -First 1
    if ($log_name) {
        $base_name = [System.IO.Path]::GetFileNameWithoutExtension($log_name)
        return $base_name
    } else {
        exit 1
    }
}

function run_test {
    param (
        [string]$prefix
    )

    # Call make cleanall and make -e
    & make cleanall *> temp.log 2>&1
    & make -e >> temp.log 2>&1
    
    # Run the test
    & .\run-test.exe exhausted-run >> temp.log 2>&1
    
    $base_name = rename_log
    Write-Output $base_name
    
    # Move the log and dat files
    Move-Item temp.log "${prefix}_${base_name}.log"
    Move-Item "${base_name}.dat" "${prefix}_${base_name}.dat"

}