function rename_log {
    $log_name = Get-ChildItem -Filter *.dat | Sort-Object LastAccessTime -Descending | Select-Object -First 1
    if ($log_name) {
        $base_name = [System.IO.Path]::GetFileNameWithoutExtension($log_name)
        return $base_name
    } else {
        exit 1
    }
}
