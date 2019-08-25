#using <System.dll>
#include <SMObject.h>
#include <SMStructs.h>
#include <conio.h>
#include <windows.h>
#include <iostream>
#include <xinput.h>
#include "XBoxController.h"
#include "XInputWrapper.h"

#define WAIT_TIME 50000

using namespace std;
using namespace System;
using namespace System::Net;
using namespace System::Net::Sockets;
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

/////////////////////////////////////Remote/////////////////////////////////////////////////////////

	//set up shared memory for Laser
	SMObject RemoteObj(_TEXT("RemoteObj"), sizeof(Remote));
	Remote* RemoteSMPtr = nullptr;  //create a Laser struct type pointer

	//access shared memory and check for fault
	RemoteObj.SMAccess();
	if (RemoteObj.SMAccessError == 1)
	{
		Console::WriteLine("Shared memory access failed for Remote");
	}

	//typecast the Laser pointer
	RemoteSMPtr = (Remote*)RemoteObj.pData;



	//Create an instantiation of the wrapper
	XInputWrapper xinput;
	//XINPUT_STATE state;
	XINPUT_VIBRATION Vibration;
	GamePad::Coordinate LT(0, 0);
	GamePad::Coordinate RT(0, 0);

	//Create a new xbox controller object
	GamePad::XBoxController controller1(&xinput, 0);

	//set shutdown flag to 0
	PMSMPtr->Shutdown.Flags.Xbox = 0;

	while (PMSMPtr->Shutdown.Flags.Xbox != 1)
	{
		//set the Xbox heartbeat to high
		PMSMPtr->Heartbeats.Flags.Xbox = 1;
		
		//set the PM Heartbeat to low (for Vehicle)
		if (PMSMPtr->PMHeartbeats.Flags.Xbox == 1)
		{
			PMSMPtr->PMHeartbeats.Flags.Xbox = 0;
			WaitCount = 0;
		}
		else
		{
			WaitCount++;
			if (WaitCount == WAIT_TIME)
			{
				//PMSMPtr->Shutdown.Status = 0xFF;
				Console::WriteLine("shutting PM from Xbox");
			}
		}
		


		LT = controller1.LeftThumbLocation();
		RT = controller1.RightThumbLocation();
		controller1.SetDeadzone(10000);

		//set the desired steering in shared memory
		double LeftThumbLocation = (double)LT.GetX();
		RemoteSMPtr->SetSteering = (double)(40.0*LeftThumbLocation/32768.0);

		//std::cout << "Left thumb locations and desired steering" << LT.GetX() << '\t' << RemoteSMPtr->SetSteering << '\t' << std::endl;

		//set the desired speed in shared memory
		double RightThumbLocation = (double)RT.GetY();
		RemoteSMPtr->SetSpeed = (double)(RightThumbLocation / 32768.0);
		//std::cout << "Right thumb locations and desired steering" << RT.GetY() << '\t' << RemoteSMPtr->SetSpeed << '\t' << std::endl;

		bool Connection = 0;
		Connection = controller1.IsConnected();

		if (Connection == 0)
		{
			RemoteSMPtr->SetSteering = 0;
			RemoteSMPtr->SetSpeed = 0;
		}

		std::cout << "Desired steering: " << '\t' << RemoteSMPtr->SetSteering << '\t' << "Desired speed: " << '\t' << RemoteSMPtr->SetSpeed << std::endl;

		if (controller1.PressedA())
		{
			//shut down all modules if A is pressed
			PMSMPtr->Shutdown.Status = 0xFF;
			Sleep(1000);
		}
	}

	//if it exits the while loop then set speed and steering to 0
	RemoteSMPtr->SetSteering = 0;
	RemoteSMPtr->SetSpeed = 0;

	//Xbox is critical: shut down all
	PMSMPtr->Shutdown.Status = 0xFF;


	return 0;
}