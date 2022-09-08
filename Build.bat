xmake project -k vsxmake
xmake config -m releasedbg
xmake -y
xmake install -o distrib
xcopy /e /y distrib\bin\ build\windows\x64\releasedbg
powershell.exe -noexit -command "& {cd Code\skyrim_ui ; pnpm install; pnpm deploy:production; cmd.exe /c xcopy /e /y dist\ ..\..\build\windows\x64\releasedbg}"
pause
