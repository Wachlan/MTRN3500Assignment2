#include <SMObject.h>
#include <SMStructs.h>
#include <windows.h>
#include <conio.h>
#include <iostream>
#include <TlHelp32.h>

#define NUM_UNITS 5
#define WAIT_TIME 5

using namespace System;
using namespace System::Threading;

bool IsProcessRunning(const char* processName);
void ManualStartup(int i);

//start all other processes one by one
// Start up sequence
TCHAR* Units[10] = //
{
	TEXT("LaserModule.exe"),
	TEXT("GPSModule.exe"),
	TEXT("XboxModule.exe"),
	TEXT("VehicleModule.exe"),
	TEXT("Display.exe"),

};

// Module execution based variable declarations
STARTUPINFO s[10];
PROCESS_INFORMATION p[10];

struct WaitCounter
{
	int Laser = 0;
	int GPS = 0;
	int Xbox = 0;
	int Vehicle = 0;
	int Display = 0;
};



int main()
{

	WaitCounter WaitCount;

/////////////////////////////////////PM/////////////////////////////////////////////////////////

	//set up shared memory for process managament
	SMObject PMObj(_TEXT("PMObj"), sizeof(PM));
	PM* PMSMPtr = nullptr;  //create a PM struct type pointer

	//create shared memory and check for fault
	PMObj.SMCreate();
	if (PMObj.SMCreateError == 1)
	{
		Console::WriteLine("Shared memory creation failed for process management");
	}

	//access shared memory and check for fault
	PMObj.SMAccess();
	if (PMObj.SMAccessError == 1)
	{
		Console::WriteLine("Shared memory access failed for process management");
	}

	//typecast the PM pointer
	PMSMPtr = (PM*)PMObj.pData;
	
/////////////////////////////////////LASER/////////////////////////////////////////////////////////

	//set up shared memory for Laser
	SMObject LaserObj(_TEXT("LaserObj"), sizeof(Laser));
	Laser* LaserSMPtr = nullptr;  //create a Laser struct type pointer

	//create shared memory and check for fault
	LaserObj.SMCreate();
	if (LaserObj.SMCreateError == 1)
	{
		Console::WriteLine("Shared memory creation failed for Laser");
	}

	//access shared memory and check for fault
	LaserObj.SMAccess();
	if (LaserObj.SMAccessError == 1)
	{
		Console::WriteLine("Shared memory access failed for Laser");
	}

	//typecast the Laser pointer
	LaserSMPtr = (Laser*)LaserObj.pData;
	
	/////////////////////////////////////GPS/////////////////////////////////////////////////////////

	//set up shared memory for GPS
	SMObject GPSObj(_TEXT("GPSObj"), sizeof(GPS));
	GPS* GPSSMPtr = nullptr;  //create a GPS struct type pointer

	//create shared memory and check for fault
	GPSObj.SMCreate();
	if (GPSObj.SMCreateError == 1)
	{
		Console::WriteLine("Shared memory creation failed for GPS");
	}

	//access shared memory and check for fault
	GPSObj.SMAccess();
	if (GPSObj.SMAccessError == 1)
	{
		Console::WriteLine("Shared memory access failed for GPS");
	}

	//typecast the Laser pointer
	GPSSMPtr = (GPS*)GPSObj.pData;
	

/////////////////////////////////////Remote/////////////////////////////////////////////////////////

	//set up shared memory for Laser
	SMObject RemoteObj(_TEXT("RemoteObj"), sizeof(Remote));
	Remote* RemoteSMPtr = nullptr;  //create a Laser struct type pointer

	RemoteObj.SMCreate();
	if (RemoteObj.SMCreateError == 1)
	{
		Console::WriteLine("Shared memory creation failed for Remote");
	}

	//access shared memory and check for fault
	RemoteObj.SMAccess();
	if (RemoteObj.SMAccessError == 1)
	{
		Console::WriteLine("Shared memory access failed for Remote");
	}

	//typecast the Laser pointer
	RemoteSMPtr = (Remote*)RemoteObj.pData;




	// Starting the processes
	for (int i = 0; i < NUM_UNITS; i++)
	{
		// Check if each process is running
		if (!IsProcessRunning(Units[i]))
		{
			ZeroMemory(&s[i], sizeof(s[i]));
			s[i].cb = sizeof(s[i]);
			ZeroMemory(&p[i], sizeof(p[i]));
			// Start the child processes.

			if (!CreateProcess(NULL,   // No module name (use command line)
				Units[i],        // Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				CREATE_NEW_CONSOLE,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory
				&s[i],            // Pointer to STARTUPINFO structure
				&p[i])           // Pointer to PROCESS_INFORMATION structure
				)
			{
				printf("%s failed (%d).\n", Units[i], GetLastError());
				_getch();
				return -1;
			}
		}
		std::cout << "Started: " << Units[i] << std::endl;
		Sleep(1000);
	}

	//make sure shutdown flags are set to 0
	PMSMPtr->Shutdown.Flags.PM = 0;
	//set all heartbeats to 0
	PMSMPtr->Heartbeats.Status = 0x00;

	while (PMSMPtr->Shutdown.Flags.PM != 1)
	{
		//set PM heartbeat to 1
		PMSMPtr->PMHeartbeats.Status = 0xFF;

		//allow time for modules to respond to heartbeat
		System::Threading::Thread::Sleep(1000);
		
		if (PMSMPtr->Heartbeats.Flags.GPS == 1)
		{
			PMSMPtr->Heartbeats.Flags.GPS = 0;
			WaitCount.GPS = 0;
		}
		else 
		{
			WaitCount.GPS++;
			std::cout << WaitCount.GPS << std::endl;
			if (WaitCount.GPS == WAIT_TIME)
			{
				//non critical failure
				ManualStartup(1);
				Console::WriteLine("Restarting gps");
				WaitCount.GPS = 0;
			}
		}

		if (PMSMPtr->Heartbeats.Flags.Laser == 1)
		{
			PMSMPtr->Heartbeats.Flags.Laser = 0;
			WaitCount.Laser = 0;
		}
		else
		{
			WaitCount.Laser++;

			if (WaitCount.Laser == WAIT_TIME)
			{
				PMSMPtr->Shutdown.Status = 0xFF;
			}
		}

		if (PMSMPtr->Heartbeats.Flags.Display == 1)
		{
			PMSMPtr->Heartbeats.Flags.Display = 0;
			WaitCount.Display = 0;
		}
		else
		{
			WaitCount.Display++;

			if (WaitCount.Display == WAIT_TIME)
			{
				//critical error
				PMSMPtr->Shutdown.Status = 0xFF;
				Console::WriteLine("shutting display");
			}
		}

		if (PMSMPtr->Heartbeats.Flags.Xbox == 1)
		{
			PMSMPtr->Heartbeats.Flags.Xbox = 0;
			WaitCount.Xbox = 0;
		}
		else
		{
			WaitCount.Xbox++;

			if (WaitCount.Xbox == WAIT_TIME)
			{
				PMSMPtr->Shutdown.Status = 0xFF;
				Console::WriteLine("shutting xbox");
			}
		}
		
		if (PMSMPtr->Heartbeats.Flags.Vehicle == 1)
		{
			PMSMPtr->Heartbeats.Flags.Vehicle = 0;
			WaitCount.Vehicle = 0;
		}
		else
		{
			WaitCount.Vehicle++;

			if (WaitCount.Vehicle == WAIT_TIME)
			{
				PMSMPtr->Shutdown.Status = 0xFF;
				Console::WriteLine("shutting vehicle");
			}
		}



		if (_kbhit())
		{
			PMSMPtr->Shutdown.Status = 0xFF;
			Sleep(1000);
			break;
		}
	}

	Console::WriteLine("Program ended");
	Sleep(1000);

	return 0;
}



//Is provess running function
bool IsProcessRunning(const char* processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_stricmp(entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}


void ManualStartup(int i)
{
	// Check if each process is running
	if (!IsProcessRunning(Units[i]))
	{
		ZeroMemory(&s[i], sizeof(s[i]));
		s[i].cb = sizeof(s[i]);
		ZeroMemory(&p[i], sizeof(p[i]));
		// Start the child processes.

		if (!CreateProcess(NULL,   // No module name (use command line)
			Units[i],        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			CREATE_NEW_CONSOLE,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory
			&s[i],            // Pointer to STARTUPINFO structure
			&p[i])           // Pointer to PROCESS_INFORMATION structure
			)
		{
			printf("%s failed (%d).\n", Units[i], GetLastError());
			_getch();
			//return -1;
		}
	}
	std::cout << "Started: " << Units[i] << std::endl;
	Sleep(1000);
}