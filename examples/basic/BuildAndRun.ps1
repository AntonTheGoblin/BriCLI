Invoke-Expression "cmake -S . -B Build" -ErrorAction Stop
Invoke-Expression "cmake --build Build" -ErrorAction Stop
Invoke-Expression ".\Build\SimpleCli.exe"