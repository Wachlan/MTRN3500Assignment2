#pragma once
#using <System.dll>
#include <SMObject.h>
#include <SMStructs.h>
#include <windows.h>
#include <conio.h>
#include <iostream>


using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

ref class LaserDevice
{
public:
	LaserDevice();
	LaserDevice(System::String^ IP, System::String^ zIdentification, int port);
	~LaserDevice();
	void GetData();
	array<double>^ Range;
	array<double>^ RangeX;
	array<double>^ RangeY;
	double NumRanges;          //the number of data points

private:
	double StartAngle;         //starting angle of scan measurements
	double Resolution;         //the angular step width of measurements
	int PortNumber;            //port number of the server
	NetworkStream^ Stream;	   //handle to NetworkStream object
	System::String^ IPAddress; //IP Address of the server
	System::String^ zID;       //zID used to authenticate
};

/*
ref class LIDAR
{
private:
	double StartAngle;
	double Resolution;
	int PortNumber;
	String^ IPAddress;

public:
	int NumRanges;
	array<double>^ Ranges;
	array<double>^ RangeX;
	array<double>^ RangeY;

	LIDAR(String^ ipaddress, int port);
	bool Connect();
	void GetRanges();
	void GetData();
	*/