function run_test {
    param (
        [string]$prefix,
        [string]$result_dir
    )
    Write-Output "current dir: $PWD"

    # Call make cleanall and make -e
    & make cleanall *> temp.log 2>&1
    & make -e >> temp.log 2>&1
    
    # Run the test
    & .\run-test.exe exhausted-run >> temp.log 2>&1
    $base_name = rename_log
    $final_name = "${prefix}_$base_name"
    Write-Output $final_name
    Move-Item -Path temp.log -Destination "$final_name.log"
    Move-Item -Path "$base_name.dat" -Destination "$final_name.dat"

    collect_results ..\$result_dir
}

function collect_results {
    param (
        [string]$result_dir
    )
    # Find and copy .dat files
    Get-ChildItem -Recurse -Filter *.dat | Copy-Item -Destination $result_dir

    # Find and copy .log files
    Get-ChildItem -Recurse -Filter *.log | Copy-Item -Destination $result_dir

}
function rename_log {
    $log_name = Get-ChildItem -Filter *.dat | Sort-Object LastAccessTime -Descending | Select-Object -First 1
    if ($log_name) {
        $base_name = [System.IO.Path]::GetFileNameWithoutExtension($log_name)
        return $base_name
    } else {
        exit 1
    }
}