[CmdletBinding()]
param (
    [Parameter(Mandatory=$false,
                HelpMessage="Path to where build files should be built",
                Position=0)]
    [String]$BuildPath = "./Build",

    [Parameter(Mandatory=$false,
                HelpMessage="Optional CMake generator string",
                Position=1)]
    [Alias("G")]
    [String]$Generator
)

# Make sure we have a build directory to use
if (!(Test-Path $BuildPath))
{
    New-Item -Path $BuildPath -ItemType "directory" >$null
}
$BuildPath = Resolve-Path -Path $BuildPath -ErrorAction Stop

# If we have a generator then format it correctly.
if ( ($null -ne $Generator) -and ($Generator.Length -gt 0))
{
    $Generator = [String]::Format("-G`"{0}`"", $Generator)
}

# Invoke cmake and the underlying build engine.
Invoke-Expression "cmake -S . -B `"$BuildPath`" $Generator"
Invoke-Expression "cmake --build `"$BuildPath`""
