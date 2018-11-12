@echo off
set VisualStudioVersion=
"C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86 && "C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe" "E:/90_RobotCons/k/Software/bin/source\4AxisArm\4AxisArm\4AxisArm.sln" /p:Configuration=Release"
