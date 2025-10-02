[CmdletBinding()]
param (
    [Parameter(Mandatory=$false,
                HelpMessage="Path to where test binaries are located",
                Position=0)]
    [String]$TestPath = "./Build",

    [Parameter(Mandatory=$false,
                HelpMessage="An optional set of test names to run.",
                Position=1)]
    [Alias("t")]
    [String[]]$Tests
)

# Make sure we have a test directory to use
$TestPath = Resolve-Path -Path $TestPath -ErrorAction Stop

# Move into the build directory and run ctest
Push-Location -Path $TestPath

# If we have been given specific tests to run then run those directly.
if ($Tests.Count -gt 0)
{
    foreach ($test in $Tests) 
    {
        [string]$testExecutable = $test
        
        if ($PSVersionTable["Platform"] -ne "Unix")
        {
            $testExecutable += ".exe"
        }
        
        if (!(Test-Path -Path $testExecutable))
        {
            Write-Host "Failed to find `"$testExecutable`" executable, skipping" -ForegroundColor Red
            continue;
        }
        
        Invoke-Expression ".\$testExecutable"
    }
}
# Otherwise just run the generic ctest
else 
{
    Invoke-Expression "ctest"
}
Pop-Location