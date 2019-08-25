#using <System.dll>
#include <SMObject.h>
#include <SMStructs.h>
#include <windows.h>
#include <conio.h>
#include <iostream>

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


	//declarations
	//create handle
	TcpClient^ Client = nullptr;
	int PortNumber;
	array<unsigned char>^ SendData;
//	array<unsigned char>^ RecvData;
	System::String^ RequestScan = nullptr;
	System::String^ LaserScan = nullptr;
	System::String^ zID = "5164192\n";

	//Instantiations
	Client = gcnew TcpClient;
	PortNumber = 25000;
	//SendData = gcnew array<unsigned char>(256);

	//Actions
	//configure the client
	Client->NoDelay = true;
	Client->ReceiveTimeout = 2000;
	Client->SendTimeout = 500;
	Client->SendBufferSize = 32;
	Client->ReceiveBufferSize = 2048; //need to get the full scan, this is probably bigger than necessary

	Client->Connect("192.168.1.200", PortNumber);
	if (Client->Connected)
	{
		Console::WriteLine("Connected to vehicle");
	}

	NetworkStream^ Stream;
	Stream = Client->GetStream();

	//authenticate with zID
	array<unsigned char>^ SendzID;
	array<unsigned char>^ ReadzID;
	// unsigned char arrays of 16 bytes each are created on managed heap
	SendzID = gcnew array<unsigned char>(32);
	ReadzID = gcnew array<unsigned char>(32);
	// Convert string command to an array of unsigned char
	SendzID = System::Text::Encoding::ASCII->GetBytes(zID);

	Stream->Write(SendzID, 0, SendzID->Length);
	System::Threading::Thread::Sleep(100);
	Stream->Read(ReadzID, 0, ReadzID->Length);
	Console::WriteLine("Authenticated");

	System::Threading::Thread::Sleep(2000);

	int Flag = 0;

	//set shutdown flag to 0
	PMSMPtr->Shutdown.Flags.Vehicle = 0;

	while (PMSMPtr->Shutdown.Flags.Vehicle != 1)
	{
		//set the Vehicle heartbeat to high
		PMSMPtr->Heartbeats.Flags.Vehicle = 1;


		if (PMSMPtr->PMHeartbeats.Flags.Vehicle == 1)
		{
			PMSMPtr->PMHeartbeats.Flags.Vehicle = 0;
			WaitCount = 0;
		}
		else
		{
			WaitCount++;
			if (WaitCount == WAIT_TIME)
			{
				PMSMPtr->Shutdown.Status = 0xFF;
				//Console::WriteLine("Vehicle shutting down PM");
			}
		}
		
		//send message to set speed and steering
		SendData = gcnew array<unsigned char>(128);
		double Speed = RemoteSMPtr->SetSpeed;
		double Steer = -RemoteSMPtr->SetSteering;

		System::String^ Message = gcnew System::String("# ");
		Message = Message + Steer.ToString("F3") + " " + Speed.ToString("F3") + " " + Flag + " #";
		SendData = Encoding::ASCII->GetBytes(Message);
		Stream->Write(SendData, 0, SendData->Length);

		Flag = -Flag + 1;
	
		System::Threading::Thread::Sleep(80);
	}

	return 0;
}
