@echo off
%comspec% /c ""C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall" x86 && "C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild" "C:\Program Files (x86)\neuromeka\NRMKToolkit\MRToolkit2016\examples\RTProjectArm3\Arm3AppManager\Arm3AppManager.sln" /p:Configuration=Release"
copy "C:\Program Files (x86)\neuromeka\NRMKToolkit\MRToolkit2016\bin\vs2010\release\*.dll" "C:\Program Files (x86)\neuromeka\NRMKToolkit\MRToolkit2016\examples\RTProjectArm3\Arm3AppManager\Release\"