@echo off
%comspec% /c "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86 && "C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe" "E:/90_RobotCons/k/Software/bin/source\EEE\EEE\EEE.sln" /p:Configuration=Release"
