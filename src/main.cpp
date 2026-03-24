#include <iostream>

/*
	Go to Project -> Properties and ensure:
		Configuration: Active(Debug)											----> Change dependent on Conifg Type
		Platform: Active(x64)													----> Change dependent on Platform Type
	
	Go to Project -> Properties -> Config Properties -> General and ensure:
		Output Directory: $(ProjectDir)x64\$(Configuration)\					----> Change dependent on dir for saving final build artifacts
		Int Directory: $(ProjectDir)build\obj\$(Platform)\$(Configuration)\		----> Change dependent on dir for saving temp files during build process

	Go to Project -> Properties -> Config Properties -> Debugging and ensure:
		Working Directory: $(ProjectDir)										----> Change dependent on main working directory
*/

int main() {
	std::cout << "Server starting...\n";

	return 0;
}