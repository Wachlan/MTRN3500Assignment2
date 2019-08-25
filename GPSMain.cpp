#using <System.dll>
#include <SMObject.h>
#include <SMStructs.h>
#include <conio.h>

#define CRC32_POLYNOMIAL	0xEDB88320L
#define WAIT_TIME 5

using namespace System;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Text;

unsigned int CalculatedCRC;
unsigned long CRC32Value(int i);
unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer);

int WaitCount = 0;

#pragma pack(1)
struct GPSData     //112 bytes
{
	unsigned int Header;
	unsigned char Discards1[40];
	double Northing;
	double Easting;
	double Height;
	unsigned char Discards2[40];
	unsigned int Checksum;
};

//make GPS object
GPSData NovatelGPS;

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

	//set the GPS heartbeat to high
	PMSMPtr->Heartbeats.Flags.GPS = 1;

	/////////////////////////////////////GPS/////////////////////////////////////////////////////////

	//set up shared memory for GPS
	SMObject GPSObj(_TEXT("GPSObj"), sizeof(GPS));
	GPS* GPSSMPtr = nullptr;  //create a GPS struct type pointer

	//access shared memory and check for fault
	GPSObj.SMAccess();
	if (GPSObj.SMAccessError == 1)
	{
		Console::WriteLine("Shared memory access failed");
	}

	//typecast the Laser pointer
	GPSSMPtr = (GPS*)GPSObj.pData;

	//////////////////////////////////////declarations///////////////////////////////////////////////////////////////
	TcpClient^ Client = nullptr;
	int PortNumber;
	array<unsigned char>^ SendData;
	array<unsigned char>^ RecvData;
	System::String^ RequestScan = nullptr;
	System::String^ GPSScan = nullptr;

	///////////////////////////////////////Instantiations/////////////////////////////////////////////////////////////
	Client = gcnew TcpClient;
	PortNumber = 24000;
	RecvData = gcnew array<unsigned char>(256);

	//configure the client
	Client->NoDelay = true;
	Client->ReceiveTimeout = 2000;
	Client->SendTimeout = 500;
	Client->SendBufferSize = 32;
	Client->ReceiveBufferSize = 2048; //need to get the full scan, this is probably bigger than necessary

	System::Threading::Thread::Sleep(100);

	Client->Connect("192.168.1.200", PortNumber);
	if (Client->Connected)
	{
		Console::WriteLine("Connected to GPS");
	}

	NetworkStream^ Stream;
	Stream = Client->GetStream();



	//set shutdown flag to 0
	PMSMPtr->Shutdown.Flags.GPS = 0;

	while (PMSMPtr->Shutdown.Flags.GPS != 1)
	{
		//set the GPS heartbeat to high
		PMSMPtr->Heartbeats.Flags.GPS = 1;
		
		//set the PM Heartbeat to low (for GPS)
		if (PMSMPtr->PMHeartbeats.Flags.GPS == 1)
		{
			PMSMPtr->PMHeartbeats.Flags.GPS = 0;
			WaitCount = 0;
		}
		else
		{
			WaitCount++;
			if (WaitCount == WAIT_TIME)
			{
				PMSMPtr->Shutdown.Status = 0xFF;
				
				//Console::WriteLine("shutting PM from gps");
			}
		}

		Stream->Read(RecvData, 0, RecvData->Length);

		unsigned char* BytePtr;
		BytePtr = (unsigned char*) &NovatelGPS;
		
		for (int i = 0; i < sizeof(GPSData); i++)
		{
			*(BytePtr + i) = RecvData[i];
		}

		CalculatedCRC = CalculateBlockCRC32(108, BytePtr);

		if (CalculatedCRC == NovatelGPS.Checksum)
		{
			GPSSMPtr->Northing = NovatelGPS.Northing;
			GPSSMPtr->Easting = NovatelGPS.Easting;
			GPSSMPtr->Height = NovatelGPS.Height;
		}
		else
		{
			Console::WriteLine("Incorrect amount of data received for GPS");
		}

		Console::WriteLine("{0,10:F3} {1,10:F3} {2,10:F3} {3,2:D} {4,2:D} {5,2:D}", GPSSMPtr->Northing, GPSSMPtr->Easting, GPSSMPtr->Height, CalculatedCRC, NovatelGPS.Checksum, CalculatedCRC==NovatelGPS.Checksum);

		if (_kbhit())
		{
			break;
		}
		System::Threading::Thread::Sleep(100);
	}



	return 0;
}

unsigned long CRC32Value(int i)
{
	int j;
	unsigned long ulCRC;
	ulCRC = i;
	for (j = 8; j > 0; j--)
	{
		if (ulCRC & 1)
			ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
		else
			ulCRC >>= 1;
	}
	return ulCRC;
}

unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */
	unsigned char* ucBuffer) /* Data block */
{
	unsigned long ulTemp1;
	unsigned long ulTemp2;
	unsigned long ulCRC = 0;
	while (ulCount-- != 0)
	{
		ulTemp1 = (ulCRC >> 8) & 0x00FFFFFFL;
		ulTemp2 = CRC32Value(((int)ulCRC ^ *ucBuffer++) & 0xff);
		ulCRC = ulTemp1 ^ ulTemp2;
	}
	return(ulCRC);
}

