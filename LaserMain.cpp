#using <System.dll>
#include <SMObject.h>
#include <SMStructs.h>
#include <conio.h>
#include <windows.h>
#include <iostream>
#include "Laser.h"

#define WAIT_TIME 50000

using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

int WaitCount = 0;

int main()
{

	/////////////////////////////////////PM/////////////////////////////////////////////////////////
		//set up shared memory for process managament
	SMObject PMObj(_TEXT("PMObj"), sizeof(PM));
	PM* PMSMPtr = nullptr;  //create a PM struct type pointer

	//don't need to use create function
	//access shared memory and check for fault
	PMObj.SMAccess();
	if (PMObj.SMAccessError == 1)
	{
		Console::WriteLine("Shared memory access failed");
	}

	//typecast the PM pointer
	PMSMPtr = (PM*)PMObj.pData;


/////////////////////////////////////LASER/////////////////////////////////////////////////////////

	//set up shared memory for Laser
	SMObject LaserObj(_TEXT("LaserObj"), sizeof(Laser));
	Laser* LaserSMPtr = nullptr;  //create a Laser struct type pointer

	//access shared memory and check for fault
	LaserObj.SMAccess();
	if (LaserObj.SMAccessError == 1)
	{
		Console::WriteLine("Shared memory access failed for Laser");
	}

	//typecast the Laser pointer
	LaserSMPtr = (Laser*)LaserObj.pData;

	//create new laser object
	LaserDevice^ L1 = gcnew LaserDevice("192.168.1.200", "5164192\n", 23000);


	//set shutdown flag to 0
	PMSMPtr->Shutdown.Flags.Laser = 0;

	while (PMSMPtr->Shutdown.Flags.Laser != 1)
	{
		//set the Laser heartbeat to high
		PMSMPtr->Heartbeats.Flags.Laser = 1;
		
		//set the PM Heartbeat to low (for Laser)
		if (PMSMPtr->PMHeartbeats.Flags.Laser == 1)
		{
			PMSMPtr->PMHeartbeats.Flags.Laser = 0;
			WaitCount = 0;
		}
		else
		{
			WaitCount++;
			if (WaitCount == WAIT_TIME)
			{
				//std::cout << WaitCount << std::endl;
				PMSMPtr->Shutdown.Status = 0xFF;
				//Console::WriteLine("shutting PM from laser");
			}
		}

		//call function to get the laser data
		L1->GetData();

		if (_kbhit())
		{
			break;
		}
		System::Threading::Thread::Sleep(500);
		
	}
	return 0;
}

