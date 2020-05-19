#include "../../Common/Window.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"

#include "../CSC8503Common/NavigationGrid.h"

#include "TutorialGame.h"

//stats stuff - https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
#include "psapi.h"

#include <iostream>
#include <fstream>

using namespace NCL;
using namespace CSC8503;

static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;
static HANDLE self;

void CurrentCPUInit()
{
	SYSTEM_INFO sysInfo;
	FILETIME ftime, fsys, fuser;

	GetSystemInfo(&sysInfo);
	numProcessors = sysInfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&lastCPU, &ftime, sizeof(FILETIME));

	self = GetCurrentProcess();
	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
}

int GetCurrentCPU()
{
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	double percent;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = (sys.QuadPart - lastSysCPU.QuadPart) +
		(user.QuadPart - lastUserCPU.QuadPart);
	percent /= (now.QuadPart - lastCPU.QuadPart);
	percent /= numProcessors;
	lastCPU = now;
	lastUserCPU = user;
	lastSysCPU = sys;

	return percent * 100;
}

int main() {
	Window*w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);

	if (!w->HasInitialised()) {
		return -1;
	}	

	//TestStateMachine();
	//TestNetworking();
	//TestPathfinding();
	
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	TutorialGame* g = new TutorialGame();

	const float INTERVAL = 1.0f;
	const int PERIOD = 60;
	float intervalProgress = 0.0f;
	int periodProgress = 0;

	int cpu[60];
	int virt[60];
	int phys[60];
	int frame[60];

	bool written = false; //whether data has been written to file or not
	bool statStart = false; //whether data collection has begun

	CurrentCPUInit();

	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();

		if (dt > 1.0f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}
		
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN)) 
		{
			statStart = true;
		}

		w->SetTitle("Max Dyson - Dissertation");

		g->UpdateGame(dt);

		intervalProgress += dt;

		if (periodProgress < PERIOD && statStart)
		{
			if (intervalProgress >= INTERVAL)
			{
				std::cout << periodProgress << std::endl;
				//print data to file
				PROCESS_MEMORY_COUNTERS_EX pmc;
				GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
				SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
				SIZE_T physMemUsedByMe = pmc.WorkingSetSize;

				MEMORYSTATUSEX memInfo;
				memInfo.dwLength = sizeof(MEMORYSTATUSEX);
				GlobalMemoryStatusEx(&memInfo);
				DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
				DWORDLONG totalPhysMem = memInfo.ullTotalPhys;

				//using ints to get round figures
				physMemUsedByMe /= 1048576;
				virtualMemUsedByMe /= 1048576;
				int framerate = 1 / dt;
				int cpuPercent = GetCurrentCPU();

				phys[periodProgress] = physMemUsedByMe;
				virt[periodProgress] = virtualMemUsedByMe;
				frame[periodProgress] = framerate;
				cpu[periodProgress] = cpuPercent;

				periodProgress++;
				intervalProgress = 0.0f;
			}
		}

		else if(!written && periodProgress == 60)
		{
			written = true;
			std::ofstream frameFile("framerate.txt");
			std::ofstream cpuFile("CPU.txt");
			std::ofstream physFile("phys.txt");
			std::ofstream virtFile("virt.txt");
			if (frameFile.is_open() && cpuFile.is_open() && physFile.is_open() && virtFile.is_open())
			{
				for (int i = 0; i < PERIOD; i++)
				{
					frameFile << frame[i] << std::endl;
					cpuFile << cpu[i] << std::endl;
					physFile << phys[i] << std::endl;
					virtFile << virt[i] << std::endl;
				}
				frameFile.close();
				cpuFile.close();
				physFile.close();
				virtFile.close();

				std::cout << "data written to file\n";
			}
			else std::cout << "Unable to open file";
		}
	}
	Window::DestroyGameWindow();
}