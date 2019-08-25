//MTRN2500
//Lachlan Chow(z5164192)

#include <iostream>
#include "XBoxController.h"
#include "XInputWrapper.h"
#include <math.h>
#define MAX_JOYSTICK 32767

XINPUT_STATE state;
XINPUT_VIBRATION Vibration;

//Constructor for the XBoxController object.
GamePad::XBoxController::XBoxController(XInputWrapper* xinput, DWORD id)
{
	this->xinput = xinput; //This will copy the input argument to the class member variable xinput
	identification = id;
}


//Returns the ID of a given controller.
DWORD GamePad::XBoxController::GetControllerId()
{
	return identification;
}


// Methods which report particular aspects of the controller status.
bool GamePad::XBoxController::IsConnected()
{
	//This will get the state of the controller given the variable 'identification'
	// All XInput function calls must be made through the XInputWrapper class.
	//The XBox controllers return 'ERROR_SUCCESS' if a connection is successful.
	if (xinput->XInputGetState(identification, &state) == ERROR_SUCCESS)
	{
		return true;
	}
	else {
		return false;
	}
}

//Method to check if button A was pressed.
bool GamePad::XBoxController::PressedA()
{
	xinput->XInputGetState(identification, &state);

	//Bitwise operator '&' ensures the bitmasks will be filtered to allow for multiple button presses.
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
	{
		return true;
	}
	else {
		return false;
	}
}


//Method to check if button B was pressed.
bool GamePad::XBoxController::PressedB()
{
	xinput->XInputGetState(identification, &state);

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
	{
		return true;
	}
	else {
		return false;
	}
}

//Method to check if button X was pressed.
bool GamePad::XBoxController::PressedX()
{
	xinput->XInputGetState(identification, &state);

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
	{
		return true;
	}
	else {
		return false;
	}
}

//Method to check if button Y was pressed.
bool GamePad::XBoxController::PressedY()
{
	xinput->XInputGetState(identification, &state);

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
	{
		return true;
	}
	else {
		return false;
	}
}

//Method to check if the left shoulder was pressed.
bool GamePad::XBoxController::PressedLeftShoulder()
{
	xinput->XInputGetState(identification, &state);

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
	{
		return true;
	}
	else {
		return false;
	}
}

//Method to check if the right shoulder was pressed.
bool GamePad::XBoxController::PressedRightShoulder()
{
	xinput->XInputGetState(identification, &state);

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
	{
		return true;
	}
	else {
		return false;
	}
}

//Method to check if the left Dpad was pressed.
bool GamePad::XBoxController::PressedLeftDpad()
{
	xinput->XInputGetState(identification, &state);

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
	{
		return true;
	}
	else {
		return false;
	}
}

//Method to check if the right Dpad was pressed.
bool GamePad::XBoxController::PressedRightDpad()
{
	xinput->XInputGetState(identification, &state);

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
	{
		return true;
	}
	else {
		return false;
	}
}

//Method to check if the up Dpad was pressed.
bool GamePad::XBoxController::PressedUpDpad()
{
	xinput->XInputGetState(identification, &state);

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
	{
		return true;
	}
	else {
		return false;
	}
}

//Method to check if the down Dpad was pressed.
bool GamePad::XBoxController::PressedDownDpad()
{
	xinput->XInputGetState(identification, &state);

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
	{
		return true;
	}
	else {
		return false;
	}
}

//Method to check if the start button was pressed.
bool GamePad::XBoxController::PressedStart()
{
	xinput->XInputGetState(identification, &state);

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START)
	{
		return true;
	}
	else {
		return false;
	}
}

//Method to check if the back button was pressed.
bool GamePad::XBoxController::PressedBack()
{
	xinput->XInputGetState(identification, &state);

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
	{
		return true;
	}
	else {
		return false;
	}
}

//Method to check if the left thumb was pressed.
bool GamePad::XBoxController::PressedLeftThumb()
{
	xinput->XInputGetState(identification, &state);

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
	{
		return true;
	}
	else {
		return false;
	}
}

//Method to check if the right thumb was pressed.
bool GamePad::XBoxController::PressedRightThumb()
{
	xinput->XInputGetState(identification, &state);

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
	{
		return true;
	}
	else {
		return false;
	}
}

//Method to return the location of the left trigger.
BYTE GamePad::XBoxController::LeftTriggerLocation()
{
	return state.Gamepad.bLeftTrigger;
}

//Method to return the location of the right trigger.
BYTE GamePad::XBoxController::RightTriggerLocation()
{
	return state.Gamepad.bRightTrigger;
}

//Method to return the location of the left thumb joystick.
GamePad::Coordinate GamePad::XBoxController::LeftThumbLocation()
{
	//Establish a new co-ordinate LT
	GamePad::Coordinate LT(0, 0);

	//Initialise the x and y components of this co-ordinate to (0,0)
	SHORT x = 0;
	SHORT y = 0;

	//Set the x and y components of this co-ordinate to the values of the controller
	x = state.Gamepad.sThumbLX;
	y = state.Gamepad.sThumbLY;

	//Do not scale the value if there is no deadzone
	if (dead != 0)
	{
		//If there is a deadzone, call the ScaleValue function to scale the value
		LT = GamePad::XBoxController::ScaleValue(x, y);
	}
	else
	{
		LT.SetX(x);
		LT.SetY(y);
	}

	//Return the co-ordinate
	return LT;
}

//Refer to previous method.
GamePad::Coordinate GamePad::XBoxController::RightThumbLocation()
{
	GamePad::Coordinate RT(0, 0);

	SHORT x = 0;
	SHORT y = 0;

	x = state.Gamepad.sThumbRX;
	y = state.Gamepad.sThumbRY;

	if (dead != 0)
	{
		RT = GamePad::XBoxController::ScaleValue(x, y);
	}
	else
	{
		RT.SetX(x);
		RT.SetY(y);
	}

	return RT;
}


//Method to vibrate the left and right motors within a controller.
void GamePad::XBoxController::Vibrate(WORD left, WORD right)
{
	//Set the motor speeds to given values.
	Vibration.wLeftMotorSpeed = left;
	Vibration.wRightMotorSpeed = right;
	xinput->XInputSetState(identification, &Vibration);
}

//Method to set the variable 'dead' to a given radius for the deadzone
void GamePad::XBoxController::SetDeadzone(unsigned int radius)
{
	dead = radius;
}

//Method to scale the co-ordinates for a given deadzone.
GamePad::Coordinate GamePad::XBoxController::ScaleValue(SHORT x, SHORT y)
{
	GamePad::Coordinate Coord(0, 0);

	double magnitude = 0;
	double percentage = 0;
	SHORT newx = 0;
	SHORT newy = 0;


	//Only perform the calculation if the co-ordinate is outside of the deadzone.
	if (x * x + y * y > dead * dead)
	{
		//Obtain the magnitude of the original co-ordinates using Pythagoras' theorem.
		magnitude = sqrt(x * x + y * y);
		percentage = MAX_JOYSTICK * ((magnitude - dead) / (MAX_JOYSTICK - dead)); //This obtains the proportion of the magnitude in relation to the co-ordinate range

		//These make sure the maximum value cannot be exceeded.
		if (magnitude > MAX_JOYSTICK)
		{
			magnitude = MAX_JOYSTICK;
		}

		if (percentage > MAX_JOYSTICK)
		{
			percentage = MAX_JOYSTICK;
		}

		//Sets the scaled x and y values by obtaining the individual components of the scaled magnitude.
		newx = percentage * (x / magnitude);
		newy = percentage * (y / magnitude);

		//Set the scaled x and y co-ordinates for the co-ordinate.
		Coord.SetX(newx);
		Coord.SetY(newy);

	}
	else
	{
		//If the co-ordinates are within the deadzone, then set them to (0,0)
		Coord.SetX(0);
		Coord.SetY(0);
	}

	return Coord;
}

