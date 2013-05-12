[CmdletBinding()]
param()

# TODO: 

# == About this program ==
# This PowerShell program invokes the test harness that runs the lr-libc unit test cases.

# == Who should use this program? ==
# This program should only be used by people who modify the function library, so *users* of the
# library should never need to run this.

# == Requirements ==
# This program requires that Windows PowerShell and HP VuGen are both installed. PowerShell should
# already be present on most modern Windows versions.
# Note: the mmdrv_path must be updated if LoadRunner/VuGen has been installed in a different
# directory.
$mmdrv_path = "C:\Program Files\HP\LoadRunner\bin"

# == Running the Test Harness ==
# Before running the PowerShell script, you will need to set the PowerShell
# Execution Policy to "RemoteSigned".
# * If you run "powershell Get-ExecutionPolicy", you will probably see that the policy is set to
#   "Restricted". If you try to run your script you will get an error saying "run_test_harness.ps1
#   cannot be loaded because the execution of scripts is disabled on this system".
# * Run "powershell Set-ExecutionPolicy RemoteSigned" to change to execution policy to one that
#   will allow you to run your script.
# Run the program from the Command Prompt from the \lr-libc\tests folder like: 
# C:\lr-libc\tests>powershell ./run_test_harness.ps1
# If there are problems with the script, debug information is available:
# C:\lr-libc\tests>powershell ./run_test_harness.ps1 -Verbose

# == How does the Test Harness work? ==
# Each unit test is in its own *.c file, and is invoked by running the test_harness VuGen script
# with mmdrv.exe (no GUI is displayed).
# The compilation log and the replay log are checked for errors, and the for the expected output
# values.

# == Future Changes ==
# * It might be useful to print a total of the number of passed/failed unit
#   tests at the end.
# * Add a flag to fail the entire test set if a single test fails.
# * re-format with 100 columns instead of 80.

################################################################################

# Check that all required files and folders are present.
$working_dir = Split-Path -Path $MyInvocation.MyCommand.Path -Parent
$replay_log = "$working_dir\output.txt"
if ( !(Test-Path $replay_log -PathType Leaf) ) {
    Write-Host "Error: '$replay_log' not found."
    return
}
$replay_log_contents = Get-Content -Path $replay_log -ErrorAction SilentlyContinue
if ( !$replay_log_contents ) {
    Write-Host "Error: Replay Log is empty."
    return 
}
$mmdrv_exe = "$mmdrv_path\mmdrv.exe"
if ( !(Test-Path $mmdrv_exe -PathType Leaf) ) {
    Write-Host "Error: mmdrv.exe not found.Check that the specified path exists: '$mmdrv_exe'."
    return
}
$test_harness = "$working_dir\test_harness"
if ( !(Test-Path "$test_harness\Action.c" -PathType Leaf) ) {
    Write-Host "Error: test_harness VuGen script not found at '$test_harness'."
    return
}

# Run each test case in the \lr-libc\tests directory.
$files = Get-ChildItem -Path "$working_dir\*.c"
#$files | Get-Member
if ( $files.count -eq 0 ) {
    Write-Host "Error. No *.c files found in '$working_dir'"
    return
} 
Write-Verbose "[$((Get-Date).ToString())] $($files.count) test cases found in $working_dir"
$num_tests_passed = 0
$num_tests_failed = 0
foreach ( $test_case in $files ) {
    Write-Verbose "[$((Get-Date).ToString())] Executing test case: '$($test_case.Name)'."
    
    # Extract all the CHECKs from the test case.
    $checks = @(Select-String -Path $test_case.FullName -Pattern '(?s)^\s*//\s*CHECK:\s*(.*)$')
    if ( !$checks ) {
        Write-Host "Error, no CHECKs found in '$($test_case.Name)'."
    } else {
        Write-Verbose "[$((Get-Date).ToString())] $($checks.Count) CHECK(s) found."
        
        # Copy the test case to ./test_harness, and run the script with mmdrv.exe.
        Copy-Item $test_case "$test_harness\Action.c"
        $args = "-usr `"$test_harness\test_harness.usr`" -drv_log_file `"$replay_log`""
        Write-Host "$mmdrv_exe $args"
        # "C:\Program Files\HP\LoadRunner\bin\mmdrv.exe" -usr "C:\Documents and Settings\Administrator\My Documents\SharpDevelop Projects\lr-libc\tests\test_harness\test_harness.usr" -drv_log_file "C:\Documents and Settings\Administrator\My Documents\SharpDevelop Projects\lr-libc\tests\output.txt"
        Start-Process $mmdrv_exe -ArgumentList $args
        
        # As there is at least 1 CHECK in this test case, the default status is
        # "passed" until a CHECK is not found.
        $test_passed = $true

        # Loops through each match found in the current file
        foreach ( $check in $checks ) {
            $check_string = $check.Matches[0].Groups[1].Value
            Write-Verbose "[$((Get-Date).ToString())] Searching for '$check_string' in the Replay Log."
            
            # The current match tested was not found in the output.txt file
            if ( ! ($replay_log_contents -match $check_string) ) {
                $test_passed = $false
                Write-Host "Could not find '$check_string' for '$($test_case.Name)'."
            }
        }

        # If there are no missing results, the test have passed successfully
        if ( $test_passed ) {
            Write-Host "Test case '$($test_case.Name)' Passed."
            $num_tests_failed++
        } else {
            Write-Host "Test case '$($test_case.Name)' Failed."
            $num_tests_passed++
        }
    }
}
# Print a summary of the test cases.
Write-Host "Test Harness completed. $($files.count) tests run. $num_tests_passed passed."`
           "$num_tests_failed failed."
