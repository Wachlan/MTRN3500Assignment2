#include "GPS.h"

GPSDevice::GPSDevice()
{
}

GPSDevice::GPSDevice(System::String^ IP, int port)
{
	PortNumber = port;
	IPAddress = IP;
	
	//declare handle to TcpClient object
	TcpClient^ Client = nullptr;

	//instantiate TcpClient object and connect
	Client = gcnew TcpClient(IPAddress, PortNumber);

	//configure client
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;
	Client->SendTimeout = 500;
	Client->SendBufferSize = 32;
	Client->ReceiveBufferSize = 2048;

	if (Client->Connected)
	{
		Console::WriteLine("Connected to GPS");
	}
	System::Threading::Thread::Sleep(100);
	//get network stream object associated with client to read and write 
	Stream = Client->GetStream();



}

GPSDevice::~GPSDevice()
{
}

void GPSDevice::GetData()
{
	array<unsigned char>^ RecvData;
	System::String^ GPSScan = nullptr;
	//array<System::String^>^ Fragments = nullptr;
	RecvData = gcnew array<unsigned char>(128);

	//read data
	Stream->Read(RecvData, 0, RecvData->Length);

	//decode message into string
	GPSScan = System::Text::Encoding::ASCII->GetString(RecvData);

	Console::WriteLine(GPSScan);

	//get length of the header
	int HeaderLength = GPSScan[3];

	Northing = GPSScan[HeaderLength + 16];
	Easting = GPSScan[HeaderLength + 24];
	Height = GPSScan[HeaderLength + 32];

	//Console::WriteLine("");
	Console::WriteLine("{0,10:F3} {1,10:F3} {2,10:F3}", Northing, Easting, Height);

}



