#pragma once
#include <SMObject.h>
#include <SMStructs.h>
#include <windows.h>
#include <conio.h>
#include <iostream>

#using <System.dll>

using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

ref class GPSDevice
{
public:
	GPSDevice();
	GPSDevice(System::String^ IPAddress, int Port);
	~GPSDevice();
	void GetData();
	double Easting;      //x co-ordinate
	double Northing;     //y co-ordinate
	double Height;       //altitude of vehicle

private:
	NetworkStream^ Stream;	   //handle to NetworkStream object
	System::String^ IPAddress; //IP Address of the server
	int PortNumber;            //port number of the server
};

