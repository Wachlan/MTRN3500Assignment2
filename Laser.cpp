#include "Laser.h"

using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;


LaserDevice::LaserDevice()
{
	throw gcnew System::NotImplementedException();
}

LaserDevice::LaserDevice(System::String^ IP, System::String^ zIdentification, int port)
{
	IPAddress = IP;
	PortNumber = port;
	zID = zIdentification;
	
	//declare handle to TcpClient object
	TcpClient^ Client = nullptr;

	//instantiate TcpClient object and connect
	Client = gcnew TcpClient(IPAddress, PortNumber);

	//configure client
	Client->NoDelay = true;
	Client->ReceiveTimeout = 2000;
	Client->SendTimeout = 500;
	Client->SendBufferSize = 32;
	Client->ReceiveBufferSize = 2048;

	if (Client->Connected)
	{
		Console::WriteLine("Connected to Lidar");
	}

	//get network stream object associated with client to read and write
	Stream = Client->GetStream();

	//authenticate with zID
	array<unsigned char>^ SendzID;
	array<unsigned char>^ ReadzID;
	// unsigned char arrays of 16 bytes each are created on managed heap
	SendzID = gcnew array<unsigned char>(16);
	ReadzID = gcnew array<unsigned char>(8);
	// Convert string command to an array of unsigned char
	SendzID = System::Text::Encoding::ASCII->GetBytes(zID);

	Stream->Write(SendzID, 0, SendzID->Length);
	System::Threading::Thread::Sleep(100);
	Stream->Read(ReadzID, 0, ReadzID->Length);
	Console::WriteLine("Authenticated");

	System::Threading::Thread::Sleep(100);
}

LaserDevice::~LaserDevice()
{
	throw gcnew System::NotImplementedException();
}

void LaserDevice::GetData()
{
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


	array<unsigned char>^ SendData;
	array<unsigned char>^ RecvData;
	System::String^ RequestScan = nullptr;
	System::String^ LaserScan = nullptr;
	array<double>^ Range = nullptr;
	array<double>^ RangeX = nullptr; //we don't know how many fields there'll be yet
	array<double>^ RangeY = nullptr;
	array<System::String^>^ Fragments = nullptr;

	RequestScan = gcnew System::String("sRN LMDscandata");
	//RecvData = gcnew array<unsigned char>(2048);

	// unsigned char arrays of 16 bytes each are created on managed heap
	SendData = gcnew array<unsigned char>(16);
	RecvData = gcnew array<unsigned char>(2048);

	//encode string used to request a scan into binary
	SendData = System::Text::Encoding::ASCII->GetBytes(RequestScan);



	//send command to lidar to commence single scan
	Stream->WriteByte(0x02);    //start of stream
	Stream->Write(SendData, 0, SendData->Length);
	Stream->WriteByte(0x03);    //end of stream

	//read data
	Stream->Read(RecvData, 0, RecvData->Length);

	//decode message into string
	LaserScan = System::Text::Encoding::ASCII->GetString(RecvData);
	//Console::WriteLine(LaserScan);

	//split the laser scan data using space
	Fragments = LaserScan->Split(' ');

	//extract data
	StartAngle = System::Convert::ToInt32(Fragments[23], 16);
	Resolution = System::Convert::ToInt32(Fragments[24], 16)/10000.0;
	NumRanges = System::Convert::ToInt32(Fragments[25], 16);

	Range = gcnew array<double>(NumRanges);
	RangeX = gcnew array<double>(NumRanges);
	RangeY = gcnew array<double>(NumRanges);

	Console::WriteLine("{0,10:F3} {1,10:F3} {2,10:F3}", StartAngle, Resolution, NumRanges);

	for (int i = 0; i < NumRanges; i++)
	{
		Range[i] = System::Convert::ToInt32(Fragments[26+i], 16);
		RangeX[i] = Range[i] * Math::Sin(i * Resolution * Math::PI / 180.0);
		RangeY[i] = Range[i] * Math::Cos(i * Resolution * Math::PI / 180.0);

		LaserSMPtr->XRange[i] = RangeX[i];
		LaserSMPtr->YRange[i] = RangeY[i];
		
		Console::WriteLine("{0,10:F3} {1,10:F3} ", LaserSMPtr->XRange[i], LaserSMPtr->YRange[i]);
	}


	return;
}
