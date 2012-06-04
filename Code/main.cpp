#include "Project.hpp"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showState)
{
	Project project(instance);
	return project.Start();
}