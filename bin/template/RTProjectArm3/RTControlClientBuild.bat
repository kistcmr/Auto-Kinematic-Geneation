%comspec% /c ""C:\Program Files (x86)\neuromeka\NRMKPlatformPC\bin\eclipse\eclipse" -nosplash -data "C:\Program Files (x86)\neuromeka\NRMKToolkit\MRToolkit2016\examples\RTProjectArm3\kinWorkspace" -application org.eclipse.cdt.managedbuilder.core.headlessbuild -import "C:\Program Files (x86)\neuromeka\NRMKToolkit\MRToolkit2016\examples\RTProjectArm3\RTControlClientArm3\Arm3ControlLib""
%comspec% /c ""C:\Program Files (x86)\neuromeka\NRMKPlatformPC\bin\eclipse\eclipsec" -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data "C:\Program Files (x86)\neuromeka\NRMKToolkit\MRToolkit2016\examples\RTProjectArm3\kinWorkspace" -build Arm3ControlLib""
%comspec% /c ""C:\Program Files (x86)\neuromeka\NRMKPlatformPC\bin\eclipse\eclipse" -nosplash -data "C:\Program Files (x86)\neuromeka\NRMKToolkit\MRToolkit2016\examples\RTProjectArm3\rtWorkspace" -application org.eclipse.cdt.managedbuilder.core.headlessbuild -import "C:\Program Files (x86)\neuromeka\NRMKToolkit\MRToolkit2016\examples\RTProjectArm3\RTControlClientArm3""
%comspec% /c ""C:\Program Files (x86)\neuromeka\NRMKPlatformPC\bin\eclipse\eclipsec" -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data "C:\Program Files (x86)\neuromeka\NRMKToolkit\MRToolkit2016\examples\RTProjectArm3\rtWorkspace" -build RTControlClientArm3"